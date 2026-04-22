#include "Chat.h"
#include "Define.h"
#include "Language.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "CustomTeleport.h"
#include "ScriptMgr.h"
#include "DatabaseEnv.h"
#include "ObjectGuid.h"

#define GTS session->GetAcoreString

/* ################  загрузка таблиц ################ */
void sCustomTeleport::LoadTeleportListContainer() {
    for (sCustomTeleport::TeleportList_Container::const_iterator itr = m_TeleportList_Container.begin(); itr != m_TeleportList_Container.end(); ++itr)
        delete* itr;

    m_TeleportList_Container.clear();

    QueryResult result = CharacterDatabase.Query("SELECT id, gossip_menu, faction, name_RU, name_EN, map, position_x, position_y, position_z, orientation FROM server_menu_teleportlist ORDER BY id;");

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    if (!result)
    {
        LOG_INFO("Custom.TeleportMaster", ">> TeleportMaster: Loaded 0 'teleportlist. DB table `server_menu_teleportlist` is empty!.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        TeleportListSTR* pTele = new TeleportListSTR;
        pTele->id = fields[0].Get<uint32>();
        pTele->gossip_menu = fields[1].Get<uint8>();
        pTele->faction = fields[2].Get<uint8>();
        pTele->name_RU = fields[3].Get<std::string>();
        pTele->name_EN = fields[4].Get<std::string>();
        pTele->map = fields[5].Get<uint16>();
        pTele->position_x = fields[6].Get<float>();
        pTele->position_y = fields[7].Get<float>();
        pTele->position_z = fields[8].Get<float>();
        pTele->orientation = fields[9].Get<float>();

        m_TeleportList_Container.push_back(pTele);
        ++count;
    } while (result->NextRow());

    LOG_INFO("Custom.TeleportMaster", ">> TeleportMaster: Loaded {} teleportlist in {} ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

std::string sCustomTeleport::GetTeleportName(Player* player, TeleportListSTR* tele)
{
    return player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? tele->name_RU : tele->name_EN;
}

void sCustomTeleport::TeleportListMain(Player* player, ObjectGuid itemGuid) {
    ClearGossipMenuFor(player);
    WorldSession* session = player->GetSession();
    uint8 playerFaction = player->GetTeamId() == TEAM_ALLIANCE ? 2 : 1; // 1 = Орда, 2 = Альянс
    for (sCustomTeleport::TeleportList_Container::const_iterator itr = m_TeleportList_Container.begin(); itr != m_TeleportList_Container.end(); ++itr)
        if ((*itr)->gossip_menu == 0 && ((*itr)->faction == playerFaction || (*itr)->faction == 3))
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, GetTeleportName(player, *itr), GOSSIP_SENDER_MAIN + 4, (*itr)->id);
    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, GTS(LANG_ITEM_BACK_TO_MAIN_MENU), GOSSIP_SENDER_MAIN, 0);
    
    if (itemGuid.IsEmpty())
        player->PlayerTalkClass->SendGossipMenu(HeadMenu(player), player->GetGUID());
    else
        player->PlayerTalkClass->SendGossipMenu(HeadMenu(player), itemGuid);
}

void sCustomTeleport::GetTeleportListAfter(Player* player, uint32 action, uint8 faction, ObjectGuid itemGuid) {
    ClearGossipMenuFor(player);
    WorldSession* session = player->GetSession();
    for (sCustomTeleport::TeleportList_Container::const_iterator itr = m_TeleportList_Container.begin(); itr != m_TeleportList_Container.end(); ++itr) {
        if ((*itr)->gossip_menu != 0 && (*itr)->gossip_menu == action && ((*itr)->faction == faction || (*itr)->faction == 3)) {
            std::string name = GetTeleportName(player, *itr);
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, name, GOSSIP_SENDER_MAIN + 5, (*itr)->id);
        }
    }
    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, session->GetAcoreString(LANG_ITEM_BACK_TO_MAIN_MENU), GOSSIP_SENDER_MAIN, 1);
    
    if (itemGuid.IsEmpty())
        player->PlayerTalkClass->SendGossipMenu(HeadMenu(player), player->GetGUID());
    else
        player->PlayerTalkClass->SendGossipMenu(HeadMenu(player), itemGuid);
}

void sCustomTeleport::TeleportFunction(Player* player, uint32 i) {
    for (sCustomTeleport::TeleportList_Container::const_iterator itr = m_TeleportList_Container.begin(); itr != m_TeleportList_Container.end(); ++itr) {
        if ((*itr)->id == i) {
            player->TeleportTo((*itr)->map, (*itr)->position_x, (*itr)->position_y, (*itr)->position_z, (*itr)->orientation);
            ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_TELEPORT_SUCCESS, GetTeleportName(player, *itr).c_str());
            player->PlayerTalkClass->SendCloseGossip();
            break;
        }
    }
}

std::string sCustomTeleport::HeadMenu(Player* player) {
    std::stringstream ss;
    ss << "Телепортация по всему миру:\nТелепорт бесплатный для всех игроков.\n\n";
    ss << "|cff00ff00Выберите место назначения.|r";
    return ss.str();
}

uint32 sCustomTeleport::CalculRequiredMoney(Player* player, uint32 money) {
    /* количество рангов (1 ранг = 2% скидка) */
    uint8 count = player->GetAuraCount(Player::GetRankSystemAuraID());
    return ((money / 100) * (100 - (count * 2)));
}

std::string sCustomTeleport::ConverterMoneyToGold(Player* player, uint32 money) {
    uint32 gold = money / GOLD;
    uint32 silv = (money % GOLD) / SILVER;
    uint32 copp = (money % GOLD) % SILVER;

    std::stringstream ss;
    ss << "\nНужно заплатить: ";

    if (money == 0)
        ss << "0|TInterface\\moneyframe\\ui-coppericon:11:11:2:0|t";
    else {
        if (gold > 0)
            ss << gold << "|TInterface\\moneyframe\\ui-goldicon:11:11:2:0|t ";
        if (silv > 0)
            ss << silv << "|TInterface\\moneyframe\\ui-silvericon:11:11:2:0|t ";
        if (copp > 0)
            ss << copp << "|TInterface\\moneyframe\\ui-coppericon:11:11:2:0|t";
    }
    return ss.str();
}

std::string sCustomTeleport::ConfirmMoneyTeleport(Player* player, std::string telename) {
    std::stringstream ss;
    if (player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU)
        ss << "Вы уверены что хотите попасть в зону\n    <" << telename << ">\nза указанную сумму ниже ?\n";
    else
        ss << "Are you sure you want to get into the zone\n    <" << telename << ">\nfor the indicated amount below ?\n";
    return ss.str();
}

class CustomTeleport_World : public WorldScript
{
public:
    CustomTeleport_World() : WorldScript("CustomTeleport_World") {}

    void OnStartup() override
    {
        // создаем базу если не создано
        CharacterDatabase.DirectExecute(sCustomTeleportMgr->sql_teleportlist);

        // прогрузка телепорт мест
        LOG_INFO("Custom.TeleportMaster", ">> TeleportMaster: Loading teleport lists ...");
        sCustomTeleportMgr->LoadTeleportListContainer();
    }
};

void AddSC_CustomTeleportOrCommand()
{
    new CustomTeleport_World();
}