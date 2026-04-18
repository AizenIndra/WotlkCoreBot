/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; If not, see <http://www.gnu.org/licenses/>.
 */

 #include "Chat.h"
 #include "CommandScript.h"
 #include "GameTime.h"
 #include "Language.h"
 #include "MapMgr.h"
 #include "Player.h"
 #include "SpellAuras.h"
 #include "SpellMgr.h"
 #include "Tokenize.h"
 #include "World.h"
 #include "Util.h"
 #include "WorldConfig.h"
 #include "StringConvert.h"
 
 using namespace Acore::ChatCommands;
 
 class vip_commandscript : public CommandScript
 {
 public:
     vip_commandscript() : CommandScript("vip_commandscript") { }
 
     ChatCommandTable GetCommands() const override
     {
         static ChatCommandTable vipCommandTable =
         {
             { "time",      HandleVipTimeCommand,             SEC_PLAYER, Console::No },
             { "debuff",    HandleVipDebuffCommand,           SEC_PLAYER, Console::No },
             { "buff",      HandleVipBuffCommand,             SEC_PLAYER, Console::No },
             { "bank",      HandleVipBankCommand,             SEC_PLAYER, Console::No },
             { "mail",      HandleVipMailCommand,             SEC_PLAYER, Console::No },
             { "repair",    HandleVipRepairCommand,           SEC_PLAYER, Console::No },
             { "resettalents", HandleVipResetTalentsCommand,  SEC_PLAYER, Console::No },
             { "taxi",      HandleVipTaxiCommand,             SEC_PLAYER, Console::No },
             { "home",      HandleVipHomeCommand,             SEC_PLAYER, Console::No },
             { "capital",   HandleVipCapitalCommand,          SEC_PLAYER, Console::No },
             { "changerace", HandleVipChangeRaceCommand,     SEC_PLAYER, Console::No },
             { "customize", HandleVipCustomizeCommand,        SEC_PLAYER, Console::No },
             { "appear",    HandleVipAppearCommand,           SEC_PLAYER, Console::No },
         };
         static ChatCommandTable commandTable =
         {
             { "vip", vipCommandTable }
         };
         return commandTable;
     }
 
    static bool HandleVipTimeCommand(ChatHandler* handler)
    {
         Player* player = handler->GetSession()->GetPlayer();
         if (!player)
             return false;
         if (player->IsHardcore())
         {
             handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
             return false;
         }
         if (!player->IsPremium())
         {
             handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
             return false;
         }
         time_t now = GameTime::GetGameTime().count();
         time_t unset = player->GetPremiumUnsetdate();
         if (unset <= now)
         {
             handler->SendSysMessage(LANG_PLAYER_VIP_TIME_EXPIRED);
             return true;
         }
         time_t diff = unset - now;
         handler->PSendSysMessage(LANG_PLAYER_VIP_TIME_EXIST, secsToTimeString(diff, true));
         return true;
     }

    static bool CheckVipCommand(ChatHandler* handler, bool configEnabled)
    {
         Player* player = handler->GetSession()->GetPlayer();
         if (!player)
             return false;
         if (player->IsHardcore())
         {
             handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
             return false;
         }
         if (!player->IsPremium())
         {
             handler->SendSysMessage(LANG_PLAYER_NOT_VIP);
             return false;
         }
         if (!configEnabled)
         {
             handler->SendSysMessage(LANG_VIP_COMMAND_DISABLED);
             return false;
         }
         return true;
     }
 
     static bool HandleVipDebuffCommand(ChatHandler* handler)
     {
         if (!CheckVipCommand(handler, sWorld->getBoolConfig(CONFIG_VIP_DEBUFF)))
             return false;
         Player* player = handler->GetSession()->GetPlayer();
         if (player->InBattleground())
         {
             handler->SendSysMessage(LANG_VIP_BG);
             return false;
         }
         if (player->HasStealthAura())
         {
             handler->SendSysMessage(LANG_VIP_STEALTH);
             return false;
         }
         if (!player->IsAlive())
         {
             handler->SendSysMessage(LANG_VIP_DEAD);
             return false;
         }
         player->RemoveAppliedAuras([](AuraApplication const* aurApp) { return !aurApp->IsPositive(); });
         if (uint32 spellId = sWorld->getIntConfig(CONFIG_VIP_DEBUFF_SPELL))
         player->CastSpell(player, spellId, true);
     handler->SendSysMessage(LANG_DONE);
     return true;
 }

 static bool HandleVipBuffCommand(ChatHandler* handler)
 {
     if (!CheckVipCommand(handler, sWorld->getBoolConfig(CONFIG_VIP_BUFF)))
         return false;
     Player* player = handler->GetSession()->GetPlayer();
     if (player->InBattleground())
     {
         handler->SendSysMessage(LANG_VIP_BG);
         return false;
     }
     if (!player->IsAlive())
     {
         handler->SendSysMessage(LANG_VIP_DEAD);
         return false;
     }
     std::string_view spellsStr = sWorld->getStringConfig(CONFIG_VIP_BUFF_SPELLS);
     if (spellsStr.empty())
     {
         handler->SendSysMessage(LANG_VIP_ERROR);
         return false;
     }
     for (std::string_view token : Acore::Tokenize(spellsStr, ',', false))
     {
         // Trim leading/trailing whitespace (e.g. " 48074" from "24752, 48074")
         size_t start = token.find_first_not_of(" \t");
         if (start == std::string_view::npos)
             continue;
         size_t end = token.find_last_not_of(" \t");
         std::string_view trimmed = token.substr(start, end - start + 1);
         if (Optional<uint32> spellId = Acore::StringTo<uint32>(trimmed))
             if (sSpellMgr->GetSpellInfo(*spellId))
                 player->CastSpell(player, *spellId, true);
     }
         handler->SendSysMessage(LANG_DONE);
         return true;
     }
 
     static bool HandleVipBankCommand(ChatHandler* handler)
     {
         if (!CheckVipCommand(handler, sWorld->getBoolConfig(CONFIG_VIP_BANK)))
             return false;
         Player* player = handler->GetSession()->GetPlayer();
         if (player->InBattleground())
         {
             handler->SendSysMessage(LANG_VIP_BG);
             return false;
         }
         handler->GetSession()->SendShowBank(player->GetGUID());
         return true;
     }
 
     static bool HandleVipMailCommand(ChatHandler* handler)
     {
         if (!CheckVipCommand(handler, sWorld->getBoolConfig(CONFIG_VIP_MAIL)))
             return false;
         Player* player = handler->GetSession()->GetPlayer();
         if (player->InBattleground())
         {
             handler->SendSysMessage(LANG_VIP_BG);
             return false;
         }
         handler->GetSession()->SendShowMailBox(player->GetGUID());
         return true;
     }
 
     static bool HandleVipRepairCommand(ChatHandler* handler)
     {
         if (!CheckVipCommand(handler, sWorld->getBoolConfig(CONFIG_VIP_REPAIR)))
             return false;
         handler->SendSysMessage(LANG_VIP_IN_DEVELOPMENT);
         return false;
     }
 
     static bool HandleVipResetTalentsCommand(ChatHandler* handler)
     {
         if (!CheckVipCommand(handler, sWorld->getBoolConfig(CONFIG_VIP_RESET_TALENTS)))
             return false;
         Player* player = handler->GetSession()->GetPlayer();
         if (player->InBattleground())
         {
             handler->SendSysMessage(LANG_VIP_BG);
             return false;
         }
         if (player->GetGroup())
         {
             handler->SendSysMessage(LANG_VIP_GROUP);
             return false;
         }
         if (!player->resetTalents(true))
         {
             handler->SendSysMessage(LANG_VIP_ERROR);
             return false;
         }
         handler->SendSysMessage(LANG_DONE);
         return true;
     }
 
     static bool HandleVipTaxiCommand(ChatHandler* handler)
     {
         if (!CheckVipCommand(handler, sWorld->getBoolConfig(CONFIG_VIP_TAXI)))
             return false;
         handler->SendSysMessage(LANG_VIP_IN_DEVELOPMENT);
         return false;
     }
 
     static bool HandleVipHomeCommand(ChatHandler* handler)
     {
         if (!CheckVipCommand(handler, sWorld->getBoolConfig(CONFIG_VIP_HOME)))
             return false;
         Player* player = handler->GetSession()->GetPlayer();
         if (player->InBattleground())
         {
             handler->SendSysMessage(LANG_VIP_BG);
             return false;
         }
         if (player->IsInCombat())
         {
             handler->SendSysMessage(LANG_YOU_IN_COMBAT);
             return false;
         }
         player->TeleportToHomebind();
         handler->SendSysMessage(LANG_DONE);
         return true;
     }
 
     static bool HandleVipCapitalCommand(ChatHandler* handler)
     {
         if (!CheckVipCommand(handler, sWorld->getBoolConfig(CONFIG_VIP_CAPITAL)))
             return false;
         Player* player = handler->GetSession()->GetPlayer();
         if (player->InBattleground())
         {
             handler->SendSysMessage(LANG_VIP_BG);
             return false;
         }
         if (player->IsInCombat())
         {
             handler->SendSysMessage(LANG_YOU_IN_COMBAT);
             return false;
         }
         float x, y, z, o;
         uint32 mapId;
         if (player->GetTeamId() == TEAM_ALLIANCE)
         {
             mapId = 0;
             x = -8960.14f; y = 516.49f; z = 96.36f; o = 0.0f;
         }
         else
         {
             mapId = 1;
             x = 1629.35f; y = -4373.39f; z = 15.35f; o = 0.0f;
         }
         if (!MapMgr::IsValidMapCoord(mapId, x, y, z))
         {
             handler->SendSysMessage(LANG_VIP_ERROR);
             return false;
         }
         player->TeleportTo(mapId, x, y, z, o);
         handler->SendSysMessage(LANG_DONE);
         return true;
     }
 
     static bool HandleVipChangeRaceCommand(ChatHandler* handler)
     {
         if (!CheckVipCommand(handler, sWorld->getBoolConfig(CONFIG_VIP_CHANGE_RACE)))
             return false;
         handler->SendSysMessage(LANG_VIP_IN_DEVELOPMENT);
         return false;
     }

     static bool HandleVipCustomizeCommand(ChatHandler* handler)
     {
         if (!CheckVipCommand(handler, sWorld->getBoolConfig(CONFIG_VIP_CUSTOMIZE)))
             return false;
         handler->SendSysMessage(LANG_VIP_IN_DEVELOPMENT);
         return false;
     }

     static bool HandleVipAppearCommand(ChatHandler* handler)
     {
         if (!CheckVipCommand(handler, sWorld->getBoolConfig(CONFIG_VIP_APPEAR)))
             return false;
         handler->SendSysMessage(LANG_VIP_IN_DEVELOPMENT);
         return false;
     }
 };
 
 void AddSC_vip_commandscript()
 {
     new vip_commandscript();
 }