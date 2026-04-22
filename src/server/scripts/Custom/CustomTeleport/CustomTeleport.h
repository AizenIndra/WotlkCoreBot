#ifndef CUSTOM_TELEPORT_H
#define CUSTOM_TELEPORT_H

#include "Define.h"
#include <vector>
#include <string>

class Player;

class sCustomTeleport
{
public:
    static sCustomTeleport* instance()
    {
        static sCustomTeleport* instance = new sCustomTeleport();
        return instance;
    }

    struct TeleportListSTR
    {
        uint32 id;            // ид самой телепортации (определяем место телепорта)
        uint8  gossip_menu;   // для определение стандартного пункта (например: 0 это старт меню телепортов, 1 это столицы)
        uint8  faction;       // определяем фракцию кому доступно (1 орда 2 альянс 3 всем)
        std::string name_RU;
        std::string name_EN;
        /* координаты для телепорта */
        uint16 map; float position_x; float position_y; float position_z; float orientation;
    };

    typedef std::vector<TeleportListSTR*> TeleportList_Container;

    /* загрузка таблиц */
    void LoadTeleportListContainer();
    void TeleportListMain(Player* player, ObjectGuid itemGuid = ObjectGuid::Empty);
    void GetTeleportListAfter(Player* player, uint32 action, uint8 faction, ObjectGuid itemGuid = ObjectGuid::Empty);
    void TeleportFunction(Player* player, uint32 i);
    std::string HeadMenu(Player* player);
    uint32 CalculRequiredMoney(Player* player, uint32 money);
    std::string ConverterMoneyToGold(Player* player, uint32 money);
    std::string ConfirmMoneyTeleport(Player* player, std::string telename);
    std::string GetTeleportName(Player* player, TeleportListSTR* tele);

    TeleportList_Container m_TeleportList_Container;

    const char* sql_teleportlist = "CREATE TABLE IF NOT EXISTS `server_menu_teleportlist` ("
        "    `id` INT(10) UNSIGNED NOT NULL COMMENT 'id',"
        "    `gossip_menu` INT(3) UNSIGNED NOT NULL COMMENT 'gossip_menu',"
        "    `faction` INT(3) UNSIGNED NOT NULL COMMENT 'faction',"
        "    `name_RU` char(100),"
        "    `name_EN` char(100),"
        "    `map` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'map',"
        "    `position_x` float NOT NULL DEFAULT '0',"
        "    `position_y` float NOT NULL DEFAULT '0',"
        "    `position_z` float NOT NULL DEFAULT '0',"
        "    `orientation` float NOT NULL DEFAULT '0',"
        "    PRIMARY KEY (`name_RU`)"
        ")"
        "COMMENT='server_menu_teleportlist'"
        "ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='server_menu_teleportlist';";
};

#define sCustomTeleportMgr sCustomTeleport::instance()
#endif