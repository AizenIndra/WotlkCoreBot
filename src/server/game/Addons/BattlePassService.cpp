#include "BattlePassService.h"

#include "DatabaseEnv.h"
#include "GameTime.h"
#include "Log.h"
#include "Player.h"
#include "StringConvert.h"
#include "StringFormat.h"
#include "WorldSession.h"
#include <algorithm>
#include <ctime>
#include <map>
#include <vector>

namespace
{
    std::string const BP_PREFIX = "ASMSG_BATTLEPASS\t";
    std::string const BP_PREFIX_LEGACY = "AC_CU_SERVER_MSG\t";

    struct SeasonRow
    {
        uint32 id = 0;
        std::string category;
        std::string name;
        uint32 endTime = 0;
        uint32 premiumCost = 0;
    };

    void LogRequest(Player* player, char const* route, uint32 opcode, std::string const& payload)
    {
        if (!player || !player->GetSession())
            return;

        LOG_INFO("battlepass", "REQ {} acc={} guid={} name='{}' opcode={} payload='{}'",
            route,
            player->GetSession()->GetAccountId(),
            player->GetGUID().GetCounter(),
            player->GetName(),
            opcode,
            payload);
    }

    void Send(Player* player, uint32 opcode, std::string const& data)
    {
        if (!player)
            return;

        if (player->GetSession())
        {
            LOG_INFO("battlepass", "RESP acc={} guid={} name='{}' opcode={} data='{}'",
                player->GetSession()->GetAccountId(),
                player->GetGUID().GetCounter(),
                player->GetName(),
                opcode,
                data);
        }

        std::string const payload = std::to_string(opcode) + "^" + data;
        player->SendAddonMessage(BP_PREFIX + payload);
        // Legacy fallback for older BattlePass client builds.
        player->SendAddonMessage(BP_PREFIX_LEGACY + payload);
    }

    bool ParseRequest(std::string const& body, uint32& opcode, std::string& payload)
    {
        size_t sep = body.find('|');
        if (sep == std::string::npos)
            sep = body.find('^');
        if (sep == std::string::npos)
            return false;

        opcode = Acore::StringTo<uint32>(body.substr(0, sep)).value_or(0);
        payload = body.substr(sep + 1);
        return opcode > 0;
    }

    bool LoadSeason(std::string const& category, SeasonRow& out)
    {
        QueryResult r = WorldDatabase.Query(
            "SELECT id, category, name, end_time, premium_cost "
            "FROM battlepass_season "
            "WHERE category = '{}' AND active = 1 AND end_time > UNIX_TIMESTAMP() "
            "ORDER BY end_time DESC, id DESC LIMIT 1",
            category);
        if (!r)
            return false;
        Field* f = r->Fetch();
        out.id = f[0].Get<uint32>();
        out.category = f[1].Get<std::string>();
        out.name = f[2].Get<std::string>();
        out.endTime = f[3].Get<uint32>();
        out.premiumCost = f[4].Get<uint32>();
        return true;
    }

    uint32 GetLevelForXP(uint32 seasonId, uint32 totalXp)
    {
        uint32 level = 0;
        QueryResult r = WorldDatabase.Query(
            "SELECT tier, xp_req FROM battlepass_reward WHERE season_id = {} ORDER BY tier ASC", seasonId);
        if (!r)
            return 0;
        do
        {
            Field* f = r->Fetch();
            uint32 tier = f[0].Get<uint32>();
            uint32 xpReq = f[1].Get<uint32>();
            if (totalXp >= xpReq)
                level = tier;
            else
                break;
        } while (r->NextRow());
        return level;
    }

    void EnsureAccountProgress(uint32 accountId, SeasonRow const& season)
    {
        LoginDatabase.DirectExecute(
            "INSERT IGNORE INTO account_battlepass_progress(account_id, season_id, category, total_xp, premium_status) "
            "VALUES({}, {}, '{}', 0, 0)", accountId, season.id, season.category);
    }

    void AddAccountBattlePassXpFromTask(Player* player, SeasonRow const& season, std::string const& category,
        uint32 addXp)
    {
        if (!addXp || !player || !player->GetSession())
            return;

        uint32 accountId = player->GetSession()->GetAccountId();
        EnsureAccountProgress(accountId, season);

        uint32 maxXp = 0;
        if (QueryResult m = WorldDatabase.Query(
                "SELECT MAX(xp_req) FROM battlepass_reward WHERE season_id = {}", season.id))
            maxXp = m->Fetch()[0].Get<uint32>();

        QueryResult p = LoginDatabase.Query(
            "SELECT total_xp FROM account_battlepass_progress "
            "WHERE account_id = {} AND season_id = {} AND category = '{}' LIMIT 1",
            accountId, season.id, category);
        uint32 totalXp = p ? p->Fetch()[0].Get<uint32>() : 0;
        uint32 newTotal = std::min(totalXp + addXp, maxXp);
        if (newTotal <= totalXp)
            return;

        LoginDatabase.DirectExecute(
            "UPDATE account_battlepass_progress SET total_xp = {}, updated_at = {} "
            "WHERE account_id = {} AND season_id = {} AND category = '{}'",
            newTotal, static_cast<uint32>(GameTime::GetGameTime().count()), accountId, season.id, category);

        LOG_INFO("battlepass", "TASK_XP_GRANT acc={} guid={} name='{}' season={} category='{}' add={} total={}->{}",
            accountId,
            player->GetGUID().GetCounter(),
            player->GetName(),
            season.id,
            category,
            addXp,
            totalXp,
            newTotal);
    }

    void HandleGetBpInfo(Player* player)
    {
        for (std::string const category : {"PVE", "PVP"})
        {
            SeasonRow season;
            if (!LoadSeason(category, season))
                continue;

            std::string rewards;
            uint32 tiers = 0;
            if (QueryResult r = WorldDatabase.Query(
                    "SELECT tier, free_item, free_count, premium_item, premium_count, xp_req, reward_type "
                    "FROM battlepass_reward WHERE season_id = {} ORDER BY tier ASC", season.id))
            {
                do
                {
                    Field* f = r->Fetch();
                    uint32 freeItem = f[1].Get<uint32>();
                    uint32 freeCount = f[2].Get<uint32>();
                    uint32 premiumItem = f[3].Get<uint32>();
                    uint32 premiumCount = f[4].Get<uint32>();
                    uint32 xpReq = f[5].Get<uint32>();
                    uint32 type = f[6].Get<uint32>();
                    rewards += Acore::StringFormat("{}{},{}{},{}{},{}{},{}{},{}{}^",
                        "", freeItem, "", freeCount, "", premiumItem, "", premiumCount, "", xpReq, "", type);
                    ++tiers;
                } while (r->NextRow());
            }

            uint32 now = static_cast<uint32>(GameTime::GetGameTime().count());
            uint32 left = season.endTime > now ? season.endTime - now : 0;
            std::string data = Acore::StringFormat("{}:{}^{}^{}^{}^0^0:{}:{}",
                season.category, season.id, left, season.name, season.endTime, tiers, rewards);
            Send(player, 1, data);
        }
    }

    void HandleGetProgress(Player* player)
    {
        uint32 accountId = player->GetSession()->GetAccountId();
        for (std::string const category : {"PVE", "PVP"})
        {
            SeasonRow season;
            if (!LoadSeason(category, season))
                continue;
            EnsureAccountProgress(accountId, season);

            QueryResult r = LoginDatabase.Query(
                "SELECT total_xp, premium_status FROM account_battlepass_progress "
                "WHERE account_id = {} AND season_id = {} AND category = '{}' LIMIT 1",
                accountId, season.id, category);
            if (!r)
                continue;
            Field* f = r->Fetch();
            uint32 totalXp = f[0].Get<uint32>();
            uint32 premium = f[1].Get<uint8>();
            uint32 level = GetLevelForXP(season.id, totalXp);
            Send(player, 2, Acore::StringFormat("{}:{},{},{},{}", category, level, totalXp, premium, season.premiumCost));
        }
    }

    void HandleGetTasks(Player* player)
    {
        for (std::string const category : {"PVE", "PVP"})
        {
            SeasonRow season;
            if (!LoadSeason(category, season))
                continue;

            std::string entries;
            if (QueryResult r = WorldDatabase.Query(
                    "SELECT id, task_type, title, description, requirement, reward_xp "
                    "FROM battlepass_task WHERE season_id = {} AND category = '{}' ORDER BY task_type, id",
                    season.id, category))
            {
                std::map<std::string, std::string> perType;
                do
                {
                    Field* f = r->Fetch();
                    uint32 id = f[0].Get<uint32>();
                    std::string type = f[1].Get<std::string>();
                    std::string title = f[2].Get<std::string>();
                    std::string desc = f[3].Get<std::string>();
                    uint32 req = f[4].Get<uint32>();
                    uint32 xp = f[5].Get<uint32>();
                    for (char& c : title) if (c == ',' || c == ':') c = ';';
                    for (char& c : desc) if (c == ',' || c == ':') c = ';';
                    perType[type] += Acore::StringFormat("{},{},{},{},{},", id, title, desc, req, xp);
                } while (r->NextRow());

                bool first = true;
                for (auto const& [type, payload] : perType)
                {
                    if (!first)
                        entries += "^";
                    entries += Acore::StringFormat("{}:{}:{}", category, type, payload);
                    first = false;
                }
            }
            if (!entries.empty())
                Send(player, 3, entries);
        }
    }

    void HandleGetTaskProgress(Player* player)
    {
        uint32 guid = player->GetGUID().GetCounter();
        std::string data;
        if (QueryResult r = CharacterDatabase.Query(
                "SELECT category, task_id, current_progress, required_progress, claimed "
                "FROM character_battlepass_task_progress "
                "WHERE guid = {} ORDER BY category, task_id", guid))
        {
            bool first = true;
            do
            {
                Field* f = r->Fetch();
                if (!first)
                    data += "^";
                data += Acore::StringFormat("{}:{}:{}:{}:{}",
                    f[0].Get<std::string>(), f[1].Get<uint32>(), f[2].Get<uint32>(),
                    f[3].Get<uint32>(), f[4].Get<uint8>());
                first = false;
            } while (r->NextRow());
        }
        Send(player, 4, data);
    }

    void HandleGetClaimStatus(Player* player)
    {
        uint32 accountId = player->GetSession()->GetAccountId();
        std::string data;
        if (QueryResult r = LoginDatabase.Query(
                "SELECT category, tier, free_claimed, premium_claimed FROM account_battlepass_reward_claim "
                "WHERE account_id = {} ORDER BY category, tier", accountId))
        {
            bool first = true;
            do
            {
                Field* f = r->Fetch();
                if (!first)
                    data += "^";
                data += Acore::StringFormat("{}:{}:{}:{}",
                    f[0].Get<std::string>(), f[1].Get<uint32>(), f[2].Get<uint8>(), f[3].Get<uint8>());
                first = false;
            } while (r->NextRow());
        }
        Send(player, 5, data);
    }

    bool IsTierClaimed(uint32 accountId, uint32 seasonId, std::string const& category, uint32 tier, bool premium)
    {
        QueryResult r = LoginDatabase.Query(
            "SELECT free_claimed, premium_claimed FROM account_battlepass_reward_claim "
            "WHERE account_id = {} AND season_id = {} AND category = '{}' AND tier = {} LIMIT 1",
            accountId, seasonId, category, tier);
        if (!r)
            return false;
        Field* f = r->Fetch();
        return premium ? (f[1].Get<uint8>() == 1) : (f[0].Get<uint8>() == 1);
    }

    void SetTierClaimed(uint32 accountId, uint32 seasonId, std::string const& category, uint32 tier, bool premium)
    {
        LoginDatabase.DirectExecute(
            "INSERT IGNORE INTO account_battlepass_reward_claim(account_id, season_id, category, tier, free_claimed, premium_claimed, updated_at) "
            "VALUES({}, {}, '{}', {}, 0, 0, {})",
            accountId, seasonId, category, tier, static_cast<uint32>(GameTime::GetGameTime().count()));
        LoginDatabase.DirectExecute(
            premium
                ? "UPDATE account_battlepass_reward_claim SET premium_claimed = 1, updated_at = {} "
                  "WHERE account_id = {} AND season_id = {} AND category = '{}' AND tier = {}"
                : "UPDATE account_battlepass_reward_claim SET free_claimed = 1, updated_at = {} "
                  "WHERE account_id = {} AND season_id = {} AND category = '{}' AND tier = {}",
            static_cast<uint32>(GameTime::GetGameTime().count()), accountId, seasonId, category, tier);
    }

    bool ClaimSingleReward(Player* player, std::string const& category, uint32 tier, bool premium, std::string& err)
    {
        SeasonRow season;
        if (!LoadSeason(category, season))
        {
            err = "ERR_NO_SEASON";
            return false;
        }

        uint32 accountId = player->GetSession()->GetAccountId();
        EnsureAccountProgress(accountId, season);
        QueryResult p = LoginDatabase.Query(
            "SELECT total_xp, premium_status FROM account_battlepass_progress "
            "WHERE account_id = {} AND season_id = {} AND category = '{}' LIMIT 1",
            accountId, season.id, category);
        if (!p)
        {
            err = "ERR_BAD_REQUEST";
            return false;
        }

        Field* pf = p->Fetch();
        uint32 totalXp = pf[0].Get<uint32>();
        bool hasPremium = pf[1].Get<uint8>() == 1;
        uint32 level = GetLevelForXP(season.id, totalXp);
        if (level < tier)
            return true;
        if (premium && !hasPremium)
            return true;
        if (IsTierClaimed(accountId, season.id, category, tier, premium))
            return true;

        QueryResult reward = WorldDatabase.Query(
            "SELECT free_item, free_count, premium_item, premium_count FROM battlepass_reward "
            "WHERE season_id = {} AND tier = {} LIMIT 1", season.id, tier);
        if (!reward)
        {
            err = "ERR_BAD_TIER";
            return false;
        }

        Field* rf = reward->Fetch();
        uint32 item = premium ? rf[2].Get<uint32>() : rf[0].Get<uint32>();
        uint32 cnt = premium ? rf[3].Get<uint32>() : rf[1].Get<uint32>();
        if (item > 0 && cnt > 0 && !player->AddItem(item, cnt))
        {
            err = "ERR_BAG";
            return false;
        }

        SetTierClaimed(accountId, season.id, category, tier, premium);
        return true;
    }

    void AdvanceTasksByEvent(Player* player, std::string const& category, std::string const& eventType, uint32 targetId, uint32 delta)
    {
        if (!player || delta == 0)
            return;

        SeasonRow season;
        if (!LoadSeason(category, season))
            return;

        uint32 guid = player->GetGUID().GetCounter();
        uint32 matchedTasks = 0;
        LOG_INFO("battlepass", "TASK_EVT acc={} guid={} name='{}' season={} category='{}' event='{}' target={} delta={}",
            player->GetSession() ? player->GetSession()->GetAccountId() : 0,
            guid,
            player->GetName(),
            season.id,
            category,
            eventType,
            targetId,
            delta);

        if (QueryResult tasks = WorldDatabase.Query(
                "SELECT id, requirement, reward_xp FROM battlepass_task "
                "WHERE season_id = {} AND category = '{}' AND event_type = '{}' "
                "AND (target_id = 0 OR target_id = {}) ORDER BY id",
                season.id, category, eventType, targetId))
        {
            do
            {
                Field* tf = tasks->Fetch();
                uint32 taskId = tf[0].Get<uint32>();
                uint32 requirement = tf[1].Get<uint32>();
                uint32 rewardXp = tf[2].Get<uint32>();
                ++matchedTasks;

                uint32 oldProg = 0;
                uint32 reqCap = requirement;
                uint8 claimedOld = 0;
                if (QueryResult oldRow = CharacterDatabase.Query(
                        "SELECT current_progress, required_progress, claimed FROM character_battlepass_task_progress "
                        "WHERE guid = {} AND season_id = {} AND category = '{}' AND task_id = {} LIMIT 1",
                        guid, season.id, category, taskId))
                {
                    Field* of = oldRow->Fetch();
                    oldProg = of[0].Get<uint32>();
                    reqCap = of[1].Get<uint32>();
                    claimedOld = of[2].Get<uint8>();
                }

                CharacterDatabase.DirectExecute(
                    "INSERT IGNORE INTO character_battlepass_task_progress(guid, season_id, category, task_id, current_progress, required_progress, claimed, updated_at) "
                    "VALUES({}, {}, '{}', {}, 0, {}, 0, {})",
                    guid, season.id, category, taskId, requirement, static_cast<uint32>(GameTime::GetGameTime().count()));

                uint32 newProg = std::min(reqCap, oldProg + delta);
                bool becameComplete =
                    (!claimedOld && rewardXp > 0 && oldProg < reqCap && newProg >= reqCap);

                CharacterDatabase.DirectExecute(
                    "UPDATE character_battlepass_task_progress "
                    "SET current_progress = LEAST(required_progress, current_progress + {}), updated_at = {} "
                    "WHERE guid = {} AND season_id = {} AND category = '{}' AND task_id = {} AND claimed = 0",
                    delta, static_cast<uint32>(GameTime::GetGameTime().count()), guid, season.id, category, taskId);

                if (becameComplete)
                    AddAccountBattlePassXpFromTask(player, season, category, rewardXp);

                if (QueryResult pr = CharacterDatabase.Query(
                        "SELECT current_progress, required_progress, claimed "
                        "FROM character_battlepass_task_progress "
                        "WHERE guid = {} AND season_id = {} AND category = '{}' AND task_id = {} LIMIT 1",
                        guid, season.id, category, taskId))
                {
                    Field* pf = pr->Fetch();
                    LOG_INFO("battlepass", "TASK_EVT_APPLY guid={} season={} category='{}' task={} progress={}/{} claimed={}",
                        guid,
                        season.id,
                        category,
                        taskId,
                        pf[0].Get<uint32>(),
                        pf[1].Get<uint32>(),
                        pf[2].Get<uint8>());
                }
            } while (tasks->NextRow());
        }
        else
        {
            LOG_INFO("battlepass", "TASK_EVT_MATCH_NONE guid={} season={} category='{}' event='{}' target={}",
                guid, season.id, category, eventType, targetId);
        }

        if (matchedTasks > 0)
        {
            LOG_INFO("battlepass", "TASK_EVT_DONE guid={} season={} category='{}' event='{}' target={} matched={}",
                guid, season.id, category, eventType, targetId, matchedTasks);
        }
    }
}

namespace BattlePassService
{
    void HandleGet(Player* player, std::string const& body)
    {
        uint32 opcode = 0;
        std::string payload;
        if (!ParseRequest(body, opcode, payload))
            return;
        LogRequest(player, "GET", opcode, payload);

        switch (opcode)
        {
            case 1: HandleGetBpInfo(player); break;
            case 2: HandleGetProgress(player); break;
            case 3: HandleGetTasks(player); break;
            case 4: HandleGetTaskProgress(player); break;
            case 5:
            case 6: HandleGetClaimStatus(player); break;
            case 14:
            {
                std::string category = (payload == "PVP") ? "PVP" : "PVE";
                SeasonRow season;
                if (!LoadSeason(category, season))
                {
                    Send(player, 14, "");
                    break;
                }
                uint32 count = 0;
                std::string bundles;
                if (QueryResult r = WorldDatabase.Query(
                        "SELECT id, name, cost, xp FROM battlepass_bundle WHERE season_id = {} AND category = '{}' ORDER BY id",
                        season.id, category))
                {
                    do
                    {
                        Field* f = r->Fetch();
                        bundles += Acore::StringFormat("{},{},{},{}^",
                            f[0].Get<uint32>(), f[1].Get<std::string>(), f[2].Get<uint32>(), f[3].Get<uint32>());
                        ++count;
                    } while (r->NextRow());
                }
                Send(player, 14, Acore::StringFormat("{}:{}:{}", category, count, bundles));
                break;
            }
            case 24:
            {
                uint32 accountId = player->GetSession()->GetAccountId();
                QueryResult r = LoginDatabase.Query(
                    "SELECT reroll_count, next_cost_copper FROM account_battlepass_reroll WHERE account_id = {} LIMIT 1",
                    accountId);
                if (!r)
                    Send(player, 24, "0:10000");
                else
                {
                    Field* f = r->Fetch();
                    Send(player, 24, Acore::StringFormat("{}:{}", f[0].Get<uint32>(), f[1].Get<uint32>()));
                }
                break;
            }
            default:
                LOG_ERROR("battlepass", "Unknown GET opcode={} payload='{}'", opcode, payload);
                break;
        }
    }

    void HandlePost(Player* player, std::string const& body)
    {
        uint32 opcode = 0;
        std::string payload;
        if (!ParseRequest(body, opcode, payload))
            return;
        LogRequest(player, "POST", opcode, payload);

        switch (opcode)
        {
            case 5:
            {
                uint32 taskId = Acore::StringTo<uint32>(payload).value_or(0);
                if (!taskId)
                    return;
                CharacterDatabase.DirectExecute(
                    "UPDATE character_battlepass_task_progress SET claimed = 1 "
                    "WHERE guid = {} AND task_id = {} AND current_progress >= required_progress",
                    player->GetGUID().GetCounter(), taskId);
                break;
            }
            case 12:
            {
                size_t c1 = payload.find(':');
                if (c1 == std::string::npos)
                {
                    std::string category = payload == "PVP" ? "PVP" : "PVE";
                    SeasonRow season;
                    if (!LoadSeason(category, season))
                    {
                        Send(player, 17, "ERR_NO_SEASON");
                        break;
                    }
                    uint32 count = 0;
                    uint32 failed = 0;
                    std::string firstErr;
                    if (QueryResult r = WorldDatabase.Query(
                            "SELECT tier FROM battlepass_reward WHERE season_id = {} ORDER BY tier", season.id))
                    {
                        do
                        {
                            Field* f = r->Fetch();
                            uint32 tier = f[0].Get<uint32>();
                            std::string err;
                            bool freeOk = ClaimSingleReward(player, category, tier, false, err);
                            bool premOk = ClaimSingleReward(player, category, tier, true, err);
                            if (!freeOk || !premOk)
                            {
                                ++failed;
                                if (firstErr.empty())
                                    firstErr = err.empty() ? "ERR_BAD_REQUEST" : err;
                                LOG_ERROR("battlepass", "Claim-all skip: category='{}' tier={} error='{}'", category, tier, firstErr);
                                continue;
                            }
                            ++count;
                        } while (r->NextRow());
                    }
                    // Do not fail whole claim-all because of one problematic reward (e.g. unique/legendary).
                    if (count > 0)
                        Send(player, 17, Acore::StringFormat("ALL_OK:{}", count));
                    else
                        Send(player, 17, firstErr.empty() ? "ERR_BAD_REQUEST" : firstErr);
                    break;
                }

                std::string category = payload.substr(0, c1) == "PVP" ? "PVP" : "PVE";
                std::string right = payload.substr(c1 + 1);
                size_t c2 = right.find(':');
                if (c2 == std::string::npos)
                {
                    Send(player, 17, "ERR_BAD_REQUEST");
                    break;
                }
                uint32 tier = Acore::StringTo<uint32>(right.substr(0, c2)).value_or(0);
                bool premium = Acore::StringTo<uint32>(right.substr(c2 + 1)).value_or(0) == 1;
                if (!tier)
                {
                    Send(player, 17, "ERR_BAD_TIER");
                    break;
                }
                std::string err;
                if (!ClaimSingleReward(player, category, tier, premium, err))
                    Send(player, 17, err.empty() ? "ERR_BAD_REQUEST" : err);
                else
                    Send(player, 17, "OK");
                break;
            }
            case 13:
            {
                std::string category = payload == "PVP" ? "PVP" : "PVE";
                SeasonRow season;
                if (!LoadSeason(category, season))
                {
                    Send(player, 16, "ERR_NO_SEASON");
                    break;
                }
                uint32 accountId = player->GetSession()->GetAccountId();
                EnsureAccountProgress(accountId, season);
                QueryResult p = LoginDatabase.Query(
                    "SELECT premium_status FROM account_battlepass_progress "
                    "WHERE account_id = {} AND season_id = {} AND category = '{}' LIMIT 1",
                    accountId, season.id, category);
                if (p && p->Fetch()[0].Get<uint8>() == 1)
                {
                    Send(player, 16, "ERR_ALREADY_OWNED");
                    break;
                }
                if (!player->GetSession()->SetAccountCurrency(season.premiumCost, 1, false))
                {
                    Send(player, 16, "ERR_FUNDS");
                    break;
                }
                LoginDatabase.DirectExecute(
                    "UPDATE account_battlepass_progress SET premium_status = 1, updated_at = {} "
                    "WHERE account_id = {} AND season_id = {} AND category = '{}'",
                    static_cast<uint32>(GameTime::GetGameTime().count()), accountId, season.id, category);
                player->GetSession()->WritePurchaseToLogs(player->GetSession(), "BATTLEPASS_PREMIUM", season.id, 1, season.premiumCost, uint32(time(nullptr)));
                Send(player, 16, "OK");
                break;
            }
            case 15:
            {
                size_t c = payload.find(':');
                if (c == std::string::npos)
                {
                    Send(player, 18, "ERR_BAD_REQUEST");
                    break;
                }
                std::string category = payload.substr(0, c) == "PVP" ? "PVP" : "PVE";
                uint32 xp = Acore::StringTo<uint32>(payload.substr(c + 1)).value_or(0);
                if (xp < 1000)
                {
                    Send(player, 18, "ERR_MIN_XP");
                    break;
                }
                SeasonRow season;
                if (!LoadSeason(category, season))
                {
                    Send(player, 18, "ERR_NO_SEASON");
                    break;
                }
                uint32 accountId = player->GetSession()->GetAccountId();
                EnsureAccountProgress(accountId, season);
                QueryResult p = LoginDatabase.Query(
                    "SELECT total_xp FROM account_battlepass_progress WHERE account_id = {} AND season_id = {} AND category = '{}' LIMIT 1",
                    accountId, season.id, category);
                uint32 totalXp = p ? p->Fetch()[0].Get<uint32>() : 0;
                uint32 maxXp = 0;
                if (QueryResult m = WorldDatabase.Query("SELECT MAX(xp_req) FROM battlepass_reward WHERE season_id = {}", season.id))
                    maxXp = m->Fetch()[0].Get<uint32>();
                if (totalXp >= maxXp)
                {
                    Send(player, 18, "ERR_MAX_TIER");
                    break;
                }
                uint32 newXp = std::min(totalXp + xp, maxXp);
                uint32 spendXp = newXp - totalXp;
                uint32 cost = (spendXp + 49) / 50;
                if (!player->GetSession()->SetAccountCurrency(cost, 1, false))
                {
                    Send(player, 18, "ERR_FUNDS");
                    break;
                }
                LoginDatabase.DirectExecute(
                    "UPDATE account_battlepass_progress SET total_xp = {}, updated_at = {} "
                    "WHERE account_id = {} AND season_id = {} AND category = '{}'",
                    newXp, static_cast<uint32>(GameTime::GetGameTime().count()), accountId, season.id, category);
                player->GetSession()->WritePurchaseToLogs(player->GetSession(), "BATTLEPASS_XP", season.id, spendXp, cost, uint32(time(nullptr)));
                Send(player, 18, Acore::StringFormat("OK:{}:{}:{}", category, spendXp, cost));
                break;
            }
            case 19:
            {
                size_t c = payload.find(':');
                if (c == std::string::npos)
                {
                    Send(player, 26, "ERR_BAD_SLOT");
                    break;
                }
                std::string category = payload.substr(0, c) == "PVP" ? "PVP" : "PVE";
                uint32 slot = Acore::StringTo<uint32>(payload.substr(c + 1)).value_or(0);
                if (slot == 0 || slot > 64)
                {
                    Send(player, 26, "ERR_BAD_SLOT");
                    break;
                }
                SeasonRow season;
                if (!LoadSeason(category, season))
                {
                    Send(player, 26, "ERR_NO_SEASON");
                    break;
                }
                uint32 accountId = player->GetSession()->GetAccountId();
                LoginDatabase.DirectExecute(
                    "INSERT IGNORE INTO account_battlepass_reroll(account_id, season_id, category, reroll_count, next_cost_copper, updated_at) "
                    "VALUES({}, {}, '{}', 0, 10000, {})",
                    accountId, season.id, category, static_cast<uint32>(GameTime::GetGameTime().count()));
                QueryResult rr = LoginDatabase.Query(
                    "SELECT reroll_count, next_cost_copper FROM account_battlepass_reroll "
                    "WHERE account_id = {} AND season_id = {} AND category = '{}' LIMIT 1",
                    accountId, season.id, category);
                if (!rr)
                {
                    Send(player, 26, "ERR_BAD_REQUEST");
                    break;
                }
                Field* f = rr->Fetch();
                uint32 count = f[0].Get<uint32>();
                uint32 costCopper = f[1].Get<uint32>();
                if (player->GetMoney() < costCopper)
                {
                    Send(player, 26, "ERR_NO_MONEY");
                    break;
                }
                player->ModifyMoney(-int32(costCopper));
                uint32 newCount = count + 1;
                uint32 nextCost = costCopper + 10000;
                LoginDatabase.DirectExecute(
                    "UPDATE account_battlepass_reroll SET reroll_count = {}, next_cost_copper = {}, updated_at = {} "
                    "WHERE account_id = {} AND season_id = {} AND category = '{}'",
                    newCount, nextCost, static_cast<uint32>(GameTime::GetGameTime().count()), accountId, season.id, category);
                Send(player, 26, Acore::StringFormat("OK:{}:{}:{}:{}", category, slot, newCount, nextCost));
                break;
            }
            default:
                LOG_ERROR("battlepass", "Unknown POST opcode={} payload='{}'", opcode, payload);
                break;
        }
    }

    void OnQuestComplete(Player* player, uint32 questId)
    {
        AdvanceTasksByEvent(player, "PVE", "QUEST_COMPLETE", questId, 1);
    }

    void OnCreatureKill(Player* player, uint32 creatureEntry)
    {
        AdvanceTasksByEvent(player, "PVE", "CREATURE_KILL", creatureEntry, 1);
    }

    void OnPVPKill(Player* player)
    {
        AdvanceTasksByEvent(player, "PVP", "PVP_KILL", 0, 1);
    }

    void OnBattlegroundWin(Player* player, bool arenaWin)
    {
        AdvanceTasksByEvent(player, "PVP", arenaWin ? "ARENA_WIN" : "BG_WIN", 0, 1);
    }
}
