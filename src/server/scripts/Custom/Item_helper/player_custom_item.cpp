#include "AccountMgr.h"
#include "Chat.h"
#include "GameTime.h"
#include "Item.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "World.h"

namespace
{
constexpr uint32 ACTION_BACK = 0;
constexpr uint32 ACTION_RANK_INFO = 2;
constexpr uint32 ACTION_PREMIUM_MENU = 10;
constexpr uint32 ACTION_BONUSES_INFO = 11;
constexpr uint32 ACTION_PREMIUM_BUY = 78;
constexpr uint32 ACTION_PREMIUM_EXTEND = 79;
constexpr uint32 ACTION_PREMIUM_7 = 80;
constexpr uint32 ACTION_PREMIUM_14 = 81;
constexpr uint32 ACTION_PREMIUM_31 = 82;
constexpr uint32 ACTION_PREM_COMMANDS_INFO = 13;
constexpr uint32 ACTION_PREM_CHAR_MANAGEMENT = 14;
constexpr uint32 ACTION_PREM_CHAR_CHANGING = 15;
constexpr uint32 ACTION_PREM_REPAIR = 16;
constexpr uint32 ACTION_PREM_REMOVE_DESERTER = 17;
constexpr uint32 ACTION_PREM_REMOVE_WEAKNESS = 18;
constexpr uint32 ACTION_PREM_BUFFS = 19;
constexpr uint32 ACTION_PREM_BANK = 20;
constexpr uint32 ACTION_PREM_RESET_COOLDOWN = 22;
constexpr uint32 ACTION_PREM_DUAL_SPEC = 12;
constexpr uint32 ACTION_PREM_RESET_TALENTS = 71;
constexpr uint32 ACTION_PREM_MAX_WEAPON_SKILLS = 73;
constexpr uint32 ACTION_PREM_CHANGE_NAME = 74;
constexpr uint32 ACTION_PREM_CUSTOMIZE = 75;
constexpr uint32 ACTION_PREM_CHANGE_FACTION = 76;
constexpr uint32 ACTION_PREM_CHANGE_RACE = 77;
}

class custom_item : public ItemScript
{
public:
    custom_item() : ItemScript("custom_item") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
    {
        if (!player || !item)
            return false;

        if (!CanOpenMenu(player))
            ShowMainMenu(player, item);

        return false;
    }

    void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action) override
    {
        if (!player || !item || sender != GOSSIP_SENDER_MAIN)
            return;

        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case ACTION_BACK:
                ShowMainMenu(player, item);
                break;
            case ACTION_RANK_INFO:
                ShowRankInfo(player, item);
                break;
            case ACTION_PREMIUM_MENU:
                ShowPremiumMenu(player, item);
                break;
            case ACTION_BONUSES_INFO:
                ShowRatesAndBonuses(player, item);
                break;
            case ACTION_PREMIUM_BUY:
                ShowPremiumBuyMenu(player, item, false);
                break;
            case ACTION_PREMIUM_EXTEND:
                ShowPremiumBuyMenu(player, item, true);
                break;
            case ACTION_PREMIUM_7:
                BuyOrExtendPremium(player, item, 7, 20);
                break;
            case ACTION_PREMIUM_14:
                BuyOrExtendPremium(player, item, 14, 40);
                break;
            case ACTION_PREMIUM_31:
                BuyOrExtendPremium(player, item, 31, 80);
                break;
            case ACTION_PREM_COMMANDS_INFO:
                ShowPremiumCommandsInfo(player, item);
                break;
            case ACTION_PREM_CHAR_MANAGEMENT:
                ShowPremiumCharacterManagement(player, item);
                break;
            case ACTION_PREM_CHAR_CHANGING:
                ShowPremiumCharacterChanging(player, item);
                break;
            case ACTION_PREM_REPAIR:
                player->DurabilityRepairAll(false, 0.0f, true);
                ChatHandler(player->GetSession()).PSendSysMessage(
                    player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Экипировка починена." : "Equipment repaired.");
                ShowPremiumMenu(player, item);
                break;
            case ACTION_PREM_REMOVE_DESERTER:
                HandleRemoveDeserter(player, item);
                break;
            case ACTION_PREM_REMOVE_WEAKNESS:
                player->RemoveAura(15007);
                ShowPremiumMenu(player, item);
                break;
            case ACTION_PREM_BUFFS:
                HandlePremiumBuffs(player, item);
                break;
            case ACTION_PREM_BANK:
                CloseGossipMenuFor(player);
                player->GetSession()->SendShowBank(player->GetGUID());
                break;
            case ACTION_PREM_RESET_COOLDOWN:
                HandleResetCooldown(player, item);
                break;
            case ACTION_PREM_DUAL_SPEC:
                if (player->GetSpecsCount() == 1 && !(player->GetLevel() < sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL)))
                    player->CastSpell(player, 63680, true);
                player->CastSpell(player, 63624, true);
                ShowPremiumCharacterManagement(player, item);
                break;
            case ACTION_PREM_RESET_TALENTS:
                player->resetTalents(true);
                player->SendTalentsInfoData(false);
                ChatHandler(player->GetSession()).PSendSysMessage(
                    player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Таланты сброшены." : "Talents have been reset.");
                ShowPremiumCharacterManagement(player, item);
                break;
            case ACTION_PREM_MAX_WEAPON_SKILLS:
                player->UpdateSkillsToMaxSkillsForLevel();
                ChatHandler(player->GetSession()).PSendSysMessage(
                    player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Оружейные навыки максимальны." : "Weapon skills are now max.");
                ShowPremiumCharacterManagement(player, item);
                break;
            case ACTION_PREM_CHANGE_NAME:
                player->SetAtLoginFlag(AT_LOGIN_RENAME);
                CloseGossipMenuFor(player);
                break;
            case ACTION_PREM_CUSTOMIZE:
                player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
                CloseGossipMenuFor(player);
                break;
            case ACTION_PREM_CHANGE_FACTION:
                player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
                CloseGossipMenuFor(player);
                break;
            case ACTION_PREM_CHANGE_RACE:
                player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
                CloseGossipMenuFor(player);
                break;
            default:
                ShowMainMenu(player, item);
                break;
        }
    }

private:
    bool CanOpenMenu(Player* player)
    {
        if (!player)
            return true;

        if (player->IsInCombat() || player->IsInFlight() || player->GetMap()->IsBattlegroundOrArena() || player->HasStealthAura() || player->isDead())
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ?
                "Сейчас это невозможно." : "Now it is impossible.");
            return true;
        }

        return false;
    }

    void ShowMainMenu(Player* player, Item* item)
    {
        std::string coinsText = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ?
            "|TInterface/ICONS/INV_Misc_Coin_01:20:20:-15:0|t Ваши бонусы: " + std::to_string(player->GetSession()->GetAccountBalance()) :
            "|TInterface/ICONS/INV_Misc_Coin_01:20:20:-15:0|t Your bonuses: " + std::to_string(player->GetSession()->GetAccountBalance());

        AddGossipItemFor(player, GOSSIP_ICON_CHAT,
            coinsText,
            GOSSIP_SENDER_MAIN, ACTION_BACK);


        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1,
            player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ?
            "|TInterface/ICONS/INV_BannerPVP_02:20:20:-15:0|t Ранговая информация" :
            "|TInterface/ICONS/INV_BannerPVP_02:20:20:-15:0|t Rank information",
            GOSSIP_SENDER_MAIN, ACTION_RANK_INFO);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1,
            player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ?
            "|TInterface/ICONS/VIP:20:20:-15:0|t Премиум" :
            "|TInterface/ICONS/VIP:20:20:-15:0|t Premium",
            GOSSIP_SENDER_MAIN, ACTION_PREMIUM_MENU);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
    }

    void ShowRankInfo(Player* player, Item* item)
    {
        std::string rankText = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ?
            "|TInterface/ICONS/INV_BannerPVP_02:20:20:-15:0|t Текущий ранг: " + std::to_string(player->GetRankByExp()) :
            "|TInterface/ICONS/INV_BannerPVP_02:20:20:-15:0|t Current rank: " + std::to_string(player->GetRankByExp());
        std::string rankExpText = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ?
            "|TInterface/ICONS/INV_Misc_Note_01:20:20:-15:0|t Опыт ранга: " + std::to_string(player->GetRankPoints()) :
            "|TInterface/ICONS/INV_Misc_Note_01:20:20:-15:0|t Rank experience: " + std::to_string(player->GetRankPoints());
        std::string nextRankText = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ?
            "|TInterface/ICONS/Ability_DualWield:20:20:-15:0|t До следующего ранга: " + std::to_string(player->PointsUntilNextRank()) :
            "|TInterface/ICONS/Ability_DualWield:20:20:-15:0|t To next rank: " + std::to_string(player->PointsUntilNextRank());

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, rankText, GOSSIP_SENDER_MAIN, ACTION_RANK_INFO);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, rankExpText, GOSSIP_SENDER_MAIN, ACTION_RANK_INFO);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, nextRankText, GOSSIP_SENDER_MAIN, ACTION_RANK_INFO);

        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Обновить" : "Refresh", GOSSIP_SENDER_MAIN, ACTION_RANK_INFO);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Назад" : "Back", GOSSIP_SENDER_MAIN, ACTION_BACK);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
    }

    void ShowPremiumMenu(Player* player, Item* item)
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, BuildPremiumStatusLine(player), GOSSIP_SENDER_MAIN, ACTION_PREMIUM_MENU);

        if (player->IsPremium())
        {
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1,
                player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Продлить премиум" : "Extend premium",
                GOSSIP_SENDER_MAIN, ACTION_PREMIUM_EXTEND);
        }
        else
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1,
                player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Купить премиум" : "Buy premium",
                GOSSIP_SENDER_MAIN, ACTION_PREMIUM_BUY);

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Misc_Book_11:20:20:-15:0|t Команды VIP",
            GOSSIP_SENDER_MAIN, ACTION_PREM_COMMANDS_INFO);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_DualWield:20:20:-15:0|t Управление персонажем",
            GOSSIP_SENDER_MAIN, ACTION_PREM_CHAR_MANAGEMENT);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Shadow_Metamorphosis:20:20:-15:0|t Изменение персонажа",
            GOSSIP_SENDER_MAIN, ACTION_PREM_CHAR_CHANGING);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Hammer_20:20:20:-15:0|t Ремонт экипировки",
            GOSSIP_SENDER_MAIN, ACTION_PREM_REPAIR);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Creature_Cursed_04:20:20:-15:0|t Убрать Дезертира",
            GOSSIP_SENDER_MAIN, ACTION_PREM_REMOVE_DESERTER);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Holy_RemoveCurse:20:20:-15:0|t Убрать Слабость",
            GOSSIP_SENDER_MAIN, ACTION_PREM_REMOVE_WEAKNESS);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Holy_ArcaneIntellect:20:20:-15:0|t Выдать баффы",
            GOSSIP_SENDER_MAIN, ACTION_PREM_BUFFS);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Misc_Bag_08:20:20:-15:0|t VIP Банк",
            GOSSIP_SENDER_MAIN, ACTION_PREM_BANK);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_ChargeNegative:20:20:-15:0|t Сбросить кулдауны",
            GOSSIP_SENDER_MAIN, ACTION_PREM_RESET_COOLDOWN);

        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Назад" : "Back", GOSSIP_SENDER_MAIN, ACTION_BACK);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
    }

    void ShowPremiumBuyMenu(Player* player, Item* item, bool extendMode)
    {
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "7 дней - 20 бонусов" : "7 days - 20 bonuses", GOSSIP_SENDER_MAIN, ACTION_PREMIUM_7);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "14 дней - 40 бонусов" : "14 days - 40 bonuses", GOSSIP_SENDER_MAIN, ACTION_PREMIUM_14);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "31 день - 80 бонусов" : "31 days - 80 bonuses", GOSSIP_SENDER_MAIN, ACTION_PREMIUM_31);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Назад" : "Back", GOSSIP_SENDER_MAIN, ACTION_PREMIUM_MENU);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
    }

    void BuyOrExtendPremium(Player* player, Item* item, uint32 days, uint32 cost)
    {
        if (!player->GetSession()->SetAccountCurrency(cost, 1, false))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ?
                "Недостаточно бонусов." : "Not enough bonuses.");
            ShowPremiumBuyMenu(player, item, player->IsPremium());
            return;
        }

        uint32 accountId = player->GetSession()->GetAccountId();
        time_t now = GameTime::GetGameTime().count();
        time_t baseTime = now;
        if (AccountMgr::GetVipStatus(accountId))
        {
            time_t currentUnset = AccountMgr::GetVIPunsetDate(accountId);
            if (currentUnset > baseTime)
                baseTime = currentUnset;
        }

        time_t newUnset = baseTime + static_cast<time_t>(days) * 24 * 60 * 60;
        if (AccountMgr::GetVipStatus(accountId))
            AccountMgr::UpdateVipStatus(accountId, newUnset);
        else
            AccountMgr::SetVipStatus(accountId, newUnset);

        player->SetPremiumStatus(true);
        player->SetPremiumUnsetdate(newUnset);

        ChatHandler(player->GetSession()).PSendSysMessage(
            player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ?
            "Премиум успешно активирован/продлен." : "Premium activated/extended successfully.");
        ShowPremiumMenu(player, item);
    }

    void ShowRatesAndBonuses(Player* player, Item* item)
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT,
            player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ?
            ("Ваши бонусы: " + std::to_string(player->GetSession()->GetAccountBalance())).c_str() :
            ("Your bonuses: " + std::to_string(player->GetSession()->GetAccountBalance())).c_str(),
            GOSSIP_SENDER_MAIN, ACTION_BONUSES_INFO);

        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Обновить" : "Refresh", GOSSIP_SENDER_MAIN, ACTION_BONUSES_INFO);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Назад" : "Back", GOSSIP_SENDER_MAIN, ACTION_BACK);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
    }

    std::string BuildPremiumStatusLine(Player* player)
    {
        if (!player->IsPremium())
            return player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ?
                "|TInterface/ICONS/VIP:20:20:-15:0|t Премиум: не активен" :
                "|TInterface/ICONS/VIP:20:20:-15:0|t Premium: inactive";

        time_t now = GameTime::GetGameTime().count();
        time_t unset = player->GetPremiumUnsetdate();
        if (unset <= now)
            return player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ?
                "|TInterface/ICONS/VIP:20:20:-15:0|t Премиум: истек" :
                "|TInterface/ICONS/VIP:20:20:-15:0|t Premium: expired";

        uint32 total = static_cast<uint32>(unset - now);
        uint32 days = total / 86400;
        uint32 hours = (total % 86400) / 3600;
        uint32 mins = (total % 3600) / 60;

        if (player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU)
            return "|TInterface/ICONS/VIP:20:20:-15:0|t Осталось премиума: " + std::to_string(days) + "д " + std::to_string(hours) + "ч " + std::to_string(mins) + "м";

        return "|TInterface/ICONS/VIP:20:20:-15:0|t Premium left: " + std::to_string(days) + "d " + std::to_string(hours) + "h " + std::to_string(mins) + "m";
    }

    void ShowPremiumCommandsInfo(Player* player, Item* item)
    {
        ChatHandler chat(player->GetSession());
        if (player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU)
        {
            chat.PSendSysMessage(".vip debuff - снять Дезертира и Слабость воскрешения");
            chat.PSendSysMessage(".vip bank - открыть банк");
            chat.PSendSysMessage(".vip repair - починить экипировку");
            chat.PSendSysMessage(".vip home - телепорт домой");
            chat.PSendSysMessage(".vip capital - телепорт в столицу");
        }
        else
        {
            chat.PSendSysMessage(".vip debuff - remove Deserter and Resurrection Sickness");
            chat.PSendSysMessage(".vip bank - open bank");
            chat.PSendSysMessage(".vip repair - repair gear");
            chat.PSendSysMessage(".vip home - teleport home");
            chat.PSendSysMessage(".vip capital - teleport to capital");
        }
        ShowPremiumMenu(player, item);
    }

    void ShowPremiumCharacterManagement(Player* player, Item* item)
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Marksmanship:20:20:-15:0|t Dual spec", GOSSIP_SENDER_MAIN, ACTION_PREM_DUAL_SPEC);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Arcane_PortalDalaran:20:20:-15:0|t Reset talents", GOSSIP_SENDER_MAIN, ACTION_PREM_RESET_TALENTS);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Sword_04:20:20:-15:0|t Max weapon skills", GOSSIP_SENDER_MAIN, ACTION_PREM_MAX_WEAPON_SKILLS);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Назад" : "Back", GOSSIP_SENDER_MAIN, ACTION_PREMIUM_MENU);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
    }

    void ShowPremiumCharacterChanging(Player* player, Item* item)
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Misc_Note_05:20:20:-15:0|t Change name", GOSSIP_SENDER_MAIN, ACTION_PREM_CHANGE_NAME);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Achievement_General:20:20:-15:0|t Customize", GOSSIP_SENDER_MAIN, ACTION_PREM_CUSTOMIZE);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_BannerPVP_01:20:20:-15:0|t Change faction", GOSSIP_SENDER_MAIN, ACTION_PREM_CHANGE_FACTION);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Shadow_Metamorphosis:20:20:-15:0|t Change race", GOSSIP_SENDER_MAIN, ACTION_PREM_CHANGE_RACE);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Назад" : "Back", GOSSIP_SENDER_MAIN, ACTION_PREMIUM_MENU);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
    }

    void HandleRemoveDeserter(Player* player, Item* item)
    {
        if (player->HasAura(26013))
            player->RemoveAurasDueToSpell(26013);
        ShowPremiumMenu(player, item);
    }

    void HandlePremiumBuffs(Player* player, Item* item)
    {
        uint32 constexpr auraSize = 13;
        uint32 constexpr auras[auraSize] = { 15366, 16609, 48162, 48074, 48170, 43223, 36880, 69994, 33081, 26035, 48469, 57623, 47440 };

        if (!player->HasAura(45523))
        {
            player->RemoveAurasByType(SPELL_AURA_MOUNTED);
            for (uint32 aura : auras)
                player->AddAura(aura, player);
            player->CastSpell(player, 45523, true);
        }

        ShowPremiumMenu(player, item);
    }

    void HandleResetCooldown(Player* player, Item* item)
    {
        if (!player->HasAura(45523))
        {
            player->RemoveArenaSpellCooldowns(true);
            player->CastSpell(player, 45523, true);
        }
        ShowPremiumMenu(player, item);
    }
};

void AddSC_custom_item()
{
    new custom_item();
}
