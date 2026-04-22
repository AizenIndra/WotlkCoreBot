#include "BattlePassService.h"
#include "Player.h"
#include "PlayerScript.h"
#include "ScriptMgr.h"
#include "ScriptPCH.h"

class BattlePassProgressPlayerScript : public PlayerScript
{
public:
    BattlePassProgressPlayerScript() : PlayerScript("BattlePassProgressPlayerScript",
        {
            PLAYERHOOK_ON_PLAYER_COMPLETE_QUEST,
            PLAYERHOOK_ON_PVP_KILL
        })
    {
    }

    void OnPlayerCompleteQuest(Player* player, Quest const* quest) override
    {
        if (!player || !quest)
            return;
        BattlePassService::OnQuestComplete(player, quest->GetQuestId());
    }

    void OnPlayerPVPKill(Player* killer, Player* /*killed*/) override
    {
        if (!killer)
            return;
        BattlePassService::OnPVPKill(killer);
    }
};

class BattlePassProgressBGScript : public AllBattlegroundScript
{
public:
    BattlePassProgressBGScript() : AllBattlegroundScript("BattlePassProgressBGScript") { }

    void OnBattlegroundEndReward(Battleground* bg, Player* player, TeamId winnerTeamId) override
    {
        if (!bg || !player)
            return;
        if (player->GetTeamId() != winnerTeamId)
            return;
        BattlePassService::OnBattlegroundWin(player, bg->isArena());
    }
};

void AddSC_BattlePassProgressScripts()
{
    new BattlePassProgressPlayerScript();
    new BattlePassProgressBGScript();
}
