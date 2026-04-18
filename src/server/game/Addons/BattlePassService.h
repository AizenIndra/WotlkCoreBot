#ifndef AC_BATTLEPASS_SERVICE_H
#define AC_BATTLEPASS_SERVICE_H

#include <string>

class Player;

namespace BattlePassService
{
    void HandleGet(Player* player, std::string const& body);
    void HandlePost(Player* player, std::string const& body);
    void OnQuestComplete(Player* player, uint32 questId);
    void OnCreatureKill(Player* player, uint32 creatureEntry);
    void OnPVPKill(Player* player);
    void OnBattlegroundWin(Player* player, bool arenaWin);
}

#endif
