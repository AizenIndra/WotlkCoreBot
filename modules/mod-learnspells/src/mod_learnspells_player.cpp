#include "Player.h"

#include "mod_learnspells.h"

void LearnSpells::OnPlayerLevelChanged(Player* player, uint8 /*oldLevel*/)
{
    LearnAllSpells(player);
}

void LearnSpells::OnPlayerLogin(Player* player)
{
    LearnAllSpells(player);
}

void LearnSpells::OnPlayerLearnTalents(Player* player, uint32 /*talentId*/, uint32 /*talentRank*/, uint32 /*spellid*/)
{
    LearnAllSpells(player);
}

void LearnSpells::LearnAllSpells(Player* player)
{
    if (player->IsGameMaster() && !EnableGamemasters)
        return;

    if (player->getClass() == CLASS_DEATH_KNIGHT && player->GetMapId() == 609)
        return;

    LearnClassSpells(player);
    LearnProficiencies(player);
    LearnMounts(player);
    AddTotems(player);
}

void LearnSpells::LearnClassSpells(Player* player)
{
    if (!EnableClassSpells && !EnableFromQuests)
        return;

    for (auto& spell : SpellsList[TYPE_CLASS])
    {
        if (spell.requiresQuest == 0 && !EnableClassSpells)
            continue;

        if (spell.requiresQuest == 1 && !EnableFromQuests)
            continue;

        // raceId -1 = any race (custom races 9-30 supported via explicit int comparison)
        if (spell.raceId == -1 || spell.raceId == static_cast<int32>(player->getRace()))
            if (spell.classId == -1 || spell.classId == static_cast<int32>(player->getClass()))
                if (spell.teamId == -1 || spell.teamId == static_cast<int32>(player->GetTeamId()))
                    if (player->GetLevel() >= spell.requiredLevel)
                        if (spell.requiredSpellId == -1 || player->HasSpell(spell.requiredSpellId))
                            if (!player->HasSpell(spell.spellId))
                                player->learnSpell(spell.spellId);
    }
}

void LearnSpells::LearnProficiencies(Player* player)
{
    if (!EnableProficiencies)
        return;

    for (auto& spell : SpellsList[TYPE_PROFICIENCIES])
        if (spell.classId == -1 || spell.classId == static_cast<int32>(player->getClass()))
            if (player->GetLevel() >= spell.requiredLevel)
                if (!player->HasSpell(spell.spellId))
                    player->learnSpell(spell.spellId);
}

void LearnSpells::LearnMounts(Player* player)
{
    if (!EnableApprenticeRiding && !EnableJourneymanRiding && !EnableExpertRiding && !EnableArtisanRiding && !EnableColdWeatherFlying)
        return;

    for (auto& spell : SpellsList[TYPE_MOUNTS])
    {
        if (((spell.spellId == SPELL_APPRENTICE_RIDING || spell.requiredSpellId == SPELL_APPRENTICE_RIDING) && !EnableApprenticeRiding) ||
            ((spell.spellId == SPELL_JOURNEYMAN_RIDING || spell.requiredSpellId == SPELL_JOURNEYMAN_RIDING) && !EnableJourneymanRiding) ||
            ((spell.spellId == SPELL_EXPERT_RIDING || spell.requiredSpellId == SPELL_EXPERT_RIDING) && !EnableExpertRiding) ||
            ((spell.spellId == SPELL_ARTISAN_RIDING || spell.requiredSpellId == SPELL_ARTISAN_RIDING) && !EnableArtisanRiding) ||
            (spell.spellId == SPELL_COLD_WEATHER_FLYING && !EnableColdWeatherFlying) ||
            (spell.requiresQuest == 1 && !EnableFromQuests))
            continue;

        if (spell.raceId == -1 || spell.raceId == static_cast<int32>(player->getRace()))
            if (spell.classId == -1 || spell.classId == static_cast<int32>(player->getClass()))
                if (spell.teamId == -1 || spell.teamId == static_cast<int32>(player->GetTeamId()))
                    if (spell.requiredSpellId == -1 || player->HasSpell(spell.requiredSpellId))
                        if (player->GetLevel() >= spell.requiredLevel)
                            if (!player->HasSpell(spell.spellId))
                                player->learnSpell(spell.spellId);
    }
}

void LearnSpells::AddTotems(Player* player)
{
    if (player->getClass() != CLASS_SHAMAN)
        return;

    if (!EnableClassSpells || !EnableFromQuests)
        return;

    uint32 totems[4][3] =
    {
        {5175, 2, 4}, // Earth Totem, TotemCategory 2, Level 4
        {5176, 4, 10}, // Fire Totem, TotemCategory 4, Level 10
        {5177, 5, 20}, // Water Totem, TotemCategory 5, Level 20
        {5178, 3, 30} // Air Totem, TotemCategory 3, Level 30
    };

    for (int i = 0; i <= 3; i++)
        if (player->GetLevel() >= totems[i][2])
            if (!player->HasItemTotemCategory(totems[i][1]))
                player->AddItem(totems[i][0], 1);
}
