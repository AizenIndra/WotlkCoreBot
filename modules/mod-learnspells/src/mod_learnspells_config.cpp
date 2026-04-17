#include "Config.h"

#include "mod_learnspells.h"

void LearnSpells::OnAfterConfigLoad(bool /*reload*/)
{
    EnableGamemasters = sConfigMgr->GetOption<bool>("LearnSpells.Gamemasters", false);
    EnableClassSpells = sConfigMgr->GetOption<bool>("LearnSpells.ClassSpells", true);
    EnableProficiencies = sConfigMgr->GetOption<bool>("LearnSpells.Proficiencies", true);
    EnableFromQuests = sConfigMgr->GetOption<bool>("LearnSpells.SpellsFromQuests", true);
    EnableApprenticeRiding = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Apprentice", false);
    EnableJourneymanRiding = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Journeyman", false);
    EnableExpertRiding = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Expert", false);
    EnableArtisanRiding = sConfigMgr->GetOption<bool>("LearnSpells.Riding.Artisan", false);
    EnableColdWeatherFlying = sConfigMgr->GetOption<bool>("LearnSpells.Riding.ColdWeatherFlying", false);
}
