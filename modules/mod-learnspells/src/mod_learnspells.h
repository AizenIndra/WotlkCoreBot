#ifndef MOD_LEARNSPELLS
#define MOD_LEARNSPELLS

#include "ScriptMgr.h"

enum SpellType
{
    TYPE_CLASS                  = 0,
    TYPE_PROFICIENCIES          = 1,
    TYPE_MOUNTS                 = 2,
    TYPE_MAX                    = 3
};

enum SpellColumn
{
    SPELL_ID                    = 0,
    SPELL_REQUIRED_TEAM         = 1,
    SPELL_REQUIRED_RACE         = 2,
    SPELL_REQUIRED_CLASS        = 3,
    SPELL_REQUIRED_LEVEL        = 4,
    SPELL_REQUIRED_SPELL_ID     = 5,
    SPELL_REQUIRES_QUEST        = 6
};

struct SpellList
{
    uint32 spellId;
    int8 teamId;
    int32 raceId;   // -1 = any race (including custom races 9-30)
    int32 classId;  // -1 = any class
    uint32 requiredLevel;
    int32 requiredSpellId;
    uint8 requiresQuest;
};

enum Riding
{
    SPELL_APPRENTICE_RIDING     = 33388,
    SPELL_JOURNEYMAN_RIDING     = 33391,
    SPELL_EXPERT_RIDING         = 34090,
    SPELL_ARTISAN_RIDING        = 34091,
    SPELL_COLD_WEATHER_FLYING   = 54197
};

class LearnSpells : public PlayerScript, WorldScript
{
public:
    LearnSpells();

    // PlayerScript
    void OnPlayerLevelChanged(Player* /*player*/, uint8 /*oldLevel*/) override;
    void OnPlayerLogin(Player* /*player*/) override;
    void OnPlayerLearnTalents(Player* /*player*/, uint32 /*talentId*/, uint32 /*talentRank*/, uint32 /*spellid*/) override;

    // WorldScript
    void OnAfterConfigLoad(bool /*reload*/) override;
    void OnLoadCustomDatabaseTable() override;

private:
    bool EnableGamemasters;
    bool EnableClassSpells;
    bool EnableProficiencies;
    bool EnableFromQuests;
    bool EnableApprenticeRiding;
    bool EnableJourneymanRiding;
    bool EnableExpertRiding;
    bool EnableArtisanRiding;
    bool EnableColdWeatherFlying;

    void LearnAllSpells(Player* /*player*/);
    void LearnClassSpells(Player* /*player*/);
    void LearnProficiencies(Player* /*player*/);
    void LearnMounts(Player* /*player*/);
    void AddTotems(Player* /*player*/);

    std::array<std::vector<SpellList>, TYPE_MAX> SpellsList;
};

#endif
