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
            PLAYERHOOK_ON_CREATURE_KILL,
            PLAYERHOOK_ON_CREATURE_KILLED_BY_PET,
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

    void OnPlayerCreatureKill(Player* killer, Creature* killed) override
    {
        if (!killer || !killed)
            return;
        BattlePassService::OnCreatureKill(killer, killed->GetEntry());
    }

    void OnPlayerCreatureKilledByPet(Player* petOwner, Creature* killed) override
    {
        if (!petOwner || !killed)
            return;
        BattlePassService::OnCreatureKill(petOwner, killed->GetEntry());
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
