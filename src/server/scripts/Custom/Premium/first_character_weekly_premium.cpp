#include "AccountMgr.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "GameTime.h"
#include "Player.h"
#include "ScriptMgr.h"

class FirstCharacterWeeklyPremiumScript : public PlayerScript
{
public:
    FirstCharacterWeeklyPremiumScript() : PlayerScript("FirstCharacterWeeklyPremiumScript", { PLAYERHOOK_ON_FIRST_LOGIN }) { }

    void OnPlayerFirstLogin(Player* player) override
    {
        if (!player || !player->GetSession())
            return;

        uint32 accountId = player->GetSession()->GetAccountId();
        uint32 characterGuid = player->GetGUID().GetCounter();
        time_t now = GameTime::GetGameTime().count();
        static constexpr time_t WEEK_SECONDS = 7 * 24 * 60 * 60;
        uint32 newUnset = static_cast<uint32>(now + WEEK_SECONDS);

        std::string accountName;
        if (!AccountMgr::GetName(accountId, accountName))
            accountName = "UNKNOWN";

        std::string characterName = player->GetName();
        LoginDatabase.EscapeString(accountName);
        LoginDatabase.EscapeString(characterName);

        // Reserve one-time grant row. INSERT IGNORE protects against races/duplicates.
        LoginDatabase.Execute(
            "INSERT IGNORE INTO account_first_character_premium "
            "(account_id, account_name, character_guid, character_name, granted_at, premium_unsetdate) "
            "VALUES ({}, '{}', {}, '{}', UNIX_TIMESTAMP(), {})",
            accountId, accountName, characterGuid, characterName, newUnset);

        QueryResult lockResult = LoginDatabase.Query(
            "SELECT character_guid, premium_unsetdate FROM account_first_character_premium WHERE account_id = {} LIMIT 1",
            accountId);
        if (!lockResult)
            return;

        uint32 lockedCharacterGuid = (*lockResult)[0].Get<uint32>();
        if (lockedCharacterGuid != characterGuid)
            return;

        uint32 premiumUnsetDate = (*lockResult)[1].Get<uint32>();
        if (premiumUnsetDate == 0)
        {
            // Recover rows created by old logic where premium_unsetdate stayed 0.
            premiumUnsetDate = newUnset;
            LoginDatabase.Execute(
                "UPDATE account_first_character_premium "
                "SET premium_unsetdate = {}, granted_at = UNIX_TIMESTAMP() "
                "WHERE account_id = {}",
                premiumUnsetDate, accountId);
        }

        // Only apply to account when premium window is active.
        if (premiumUnsetDate <= static_cast<uint32>(now))
            return;

        // Do not reduce premium bought/extended by player.
        // Keep the maximum of current account premium and one-time auto premium.
        time_t currentUnset = AccountMgr::GetVIPunsetDate(accountId);
        time_t targetUnset = std::max(currentUnset, static_cast<time_t>(premiumUnsetDate));

        if (AccountMgr::GetVipStatus(accountId))
        {
            if (targetUnset > currentUnset)
                AccountMgr::UpdateVipStatus(accountId, targetUnset);
        }
        else
            AccountMgr::SetVipStatus(accountId, targetUnset);

        player->SetPremiumStatus(true);
        player->SetPremiumUnsetdate(targetUnset);
    }
};

void AddSC_first_character_weekly_premium()
{
    new FirstCharacterWeeklyPremiumScript();
}
