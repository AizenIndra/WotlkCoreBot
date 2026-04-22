/*
* Copyright (C) 2016-2019 AtieshCore <https://at-wow.org/>
* Copyright (C) 2008-2017 TrinityCore <http://www.trinitycore.org/>
* Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Chat.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "Map.h"
#include "WorldSession.h"
#include "Item.h"
#include "Language.h"
#include "InstanceSaveMgr.h"
#include "VipMountHelper.h"
#include "../CustomTeleport/CustomTeleport.h"
#include "Guild.h"
#include "DBCStores.h"
#include "GuildMgr.h"
#include "StringFormat.h"
#include "DatabaseEnv.h"
#include "Player.h"

#define CONST_ARENA_RENAME 100
#define CONST_ARENA_CUSTOMIZE 100
#define CONST_ARENA_CHANGE_FACTION 500
#define CONST_ARENA_CHANGE_RACE 250

#define GTS session->GetAcoreString

uint32 constexpr aurassSize = 13;
uint32 aurass[aurassSize] = { 15366, 16609, 48162, 48074, 48170, 43223, 36880, 69994, 33081, 26035, 48469, 57623, 47440 };

class custom_item : public ItemScript
{
public:
    custom_item() : ItemScript("custom_item") {}

private:
    static constexpr uint32 ACTION_RANK_INFO = 200;

    void ShowRankInfo(Player* player, Item* item)
    {
        if (!player)
            return;

        player->PlayerTalkClass->ClearMenus();
        std::string name = player->GetName();
        std::ostringstream femb;

        femb << "Уважаемый|cff065961 " << name << "|r\n\n"
            << "Ваш текуший ранг: |cff065961" << player->GetRankByExp() << "|r\n"
            << "У вас: |cff065961" << player->GetRankPoints() << "|r опыта\n"
            << "До следущего ранга: |cff065961" << player->PointsUntilNextRank() << "|r опыта\n\n"
            << "Ранги это вот этот значок |TInterface\\icons\\Ability_warrior_rampage:14:14:0:-1|t который отображается в дебафах.\n\n"
            << "Поднимать ранг вы можете за опыт который получите за:\n"
            << "   * Победу на арене\n   * Победу на поле боя\n   * Убийство игроков\n   * Награда за квесты\n   * Ивенты\n"
            << "У |cff065961VIP аккаунтов|r рейтинг на опыт в 2 раза больше.\n\n"
            << "Что дает ранг ?\n"
            << "На каждом ранге есть свои бонусы, чем выше ранг тем больше бонусов.\n"
            << "На каждом ранге у вас открываются секретный продавец в котором могут быть полезные вещи такие как: вещи на А9-T11, трансмогрификацию, маунты, итд...";

        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Обновить", GOSSIP_SENDER_MAIN, ACTION_RANK_INFO);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Назад", GOSSIP_SENDER_MAIN, 0);
        player->PlayerTalkClass->SendGossipMenu(femb.str().c_str(), item->GetGUID());
    }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
    {
        WorldSession* session = player->GetSession();
        player->PlayerTalkClass->ClearMenus();

        if (player->GetMap()->IsBattlegroundOrArena())
        {
            ChatHandler(player->GetSession()).PSendSysMessage(LANG_NOT_USED_BG);
            CloseGossipMenuFor(player);
            return false;
        }
        if (player->IsInCombat())
        {
            ChatHandler(player->GetSession()).PSendSysMessage(LANG_YOU_IN_COMBAT);
            CloseGossipMenuFor(player);
            return false;
        }
        if (player->IsInFlight())
        {
            ChatHandler(player->GetSession()).PSendSysMessage(LANG_YOU_IN_FLIGHT);
            CloseGossipMenuFor(player);
            return false;
        }
        if (player->HasStealthAura())
        {
            ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_NOT_USED_STEALTH);
            CloseGossipMenuFor(player);
            return false;
        }
        if (player->isDead() || player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_NOT_USED_DEAD);
            CloseGossipMenuFor(player);
            return false;
        }

        // Show main menu
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, GTS(LANG_ITEM_TELEPORT_MENU), GOSSIP_SENDER_MAIN, 100);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, GTS(LANG_RANK_SYSTEM_MENU), GOSSIP_SENDER_MAIN, 200);

        // Show guild menu if player is in guild
        if (player->GetGuild())
            AddGossipItemFor(player, GOSSIP_ICON_TABARD, GTS(LANG_GSYSTEM_GUILD_MENU), GOSSIP_SENDER_MAIN, 103);

        // Show premium menu if player is premium
        if (player->IsPremium())
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_VIP_MENU), GOSSIP_SENDER_MAIN, 10);
        else
            ChatHandler(player->GetSession()).PSendSysMessage("Некоторые функции требуют премиум статус.");

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_CLOSE), GOSSIP_SENDER_MAIN, 3);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
        return false;
    }

    void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action) override
    {
        WorldSession* session = player->GetSession();
        player->PlayerTalkClass->ClearMenus();

        if (sender == GOSSIP_SENDER_MAIN)
        {
            switch (action)
            {
            case 0: // Back from teleportation to main menu
            case 1: // Back from teleportation submenu
            {
                // Show main menu
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, GTS(LANG_ITEM_TELEPORT_MENU), GOSSIP_SENDER_MAIN, 100);
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, GTS(LANG_RANK_SYSTEM_MENU), GOSSIP_SENDER_MAIN, 200);

                // Show guild menu if player is in guild
                if (player->GetGuild())
                    AddGossipItemFor(player, GOSSIP_ICON_TABARD, GTS(LANG_GSYSTEM_GUILD_MENU), GOSSIP_SENDER_MAIN, 103);

                // Show premium menu if player is premium
                if (player->IsPremium())
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_VIP_MENU), GOSSIP_SENDER_MAIN, 10);

                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_CLOSE), GOSSIP_SENDER_MAIN, 3);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
                break;
            }
            case 3: // Close menu
            {
                CloseGossipMenuFor(player);
                break;
            }
            case 10: // VIP character management
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_COMMAND_LIST), GOSSIP_SENDER_MAIN, 13);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_CHARACTER_MANAGEMENT), GOSSIP_SENDER_MAIN, 14);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_CHARACTER_CHANGEING), GOSSIP_SENDER_MAIN, 15);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_REPAIR_EQUIP), GOSSIP_SENDER_MAIN, 16);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_REMOVE_DESERTER), GOSSIP_SENDER_MAIN, 17);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_REMOVE_WEAKNESS), GOSSIP_SENDER_MAIN, 18);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_GIVE_BUFFS), GOSSIP_SENDER_MAIN, 19);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_VIP_BANK), GOSSIP_SENDER_MAIN, 20);
                if (!player->IsInCombat() || !player->IsInFlight() || !player->GetMap()->IsBattlegroundOrArena() || !player->HasStealthAura() || !player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH) || !player->isDead())
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_REMOVE_COOLDOWN), GOSSIP_SENDER_MAIN, 22);
                if (!player->IsInCombat() || !player->IsInFlight() || !player->GetMap()->IsBattlegroundOrArena() || !player->HasStealthAura() || !player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH) || !player->isDead())
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, GTS(LANG_ITEM_RESTORE_HP_MANA), GOSSIP_SENDER_MAIN, 25);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, GTS(LANG_ITEM_RESET_SAVED_INSTANCES), GOSSIP_SENDER_MAIN, 26);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_CLOSE), GOSSIP_SENDER_MAIN, 3);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
                break;
            }
            case 100: // Teleportation main menu
            {
                sCustomTeleportMgr->TeleportListMain(player, item->GetGUID());
                break;
            }
            case 200: // Rank system info
            {
                ShowRankInfo(player, item);
                break;
            }
            case 12: // dual spec
            {
                if (player->GetSpecsCount() == 1 && !(player->GetLevel() < sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL)))
                    player->CastSpell(player, 63680, true);
                player->CastSpell(player, 63624, true);
                CloseGossipMenuFor(player);
                break;
            }
            case 13: // Vip info
            {
                ChatHandler(player->GetSession()).PSendSysMessage("|cff006699Следующие VIP команды доступны для вас:");
                if (sWorld->getBoolConfig(CONFIG_VIP_DEBUFF))
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff006699 .vip debuff|r - Снять дебаффы Дезертир и Слабость после воскрешения");
                if (sWorld->getBoolConfig(CONFIG_VIP_BANK))
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff006699 .vip bank|r - Открыть окно банка");
                if (sWorld->getBoolConfig(CONFIG_VIP_MAIL))
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff006699 .vip mail|r - Открыть почтовый ящик");
                if (sWorld->getBoolConfig(CONFIG_VIP_REPAIR))
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff006699 .vip repair|r - Починить экипировку без затрат");
                if (sWorld->getBoolConfig(CONFIG_VIP_RESET_TALENTS))
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff006699 .vip resettalents|r - Сбросить таланты");
                if (sWorld->getBoolConfig(CONFIG_VIP_TAXI))
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff006699 .vip taxi|r - Открыть окно полетов");
                if (sWorld->getBoolConfig(CONFIG_VIP_HOME))
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff006699 .vip home|r - Телепорт на точку возврата (Камень возврата)");
                if (sWorld->getBoolConfig(CONFIG_VIP_CAPITAL))
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff006699 .vip capital|r - Телепорт в главный город");
                if (sWorld->getBoolConfig(CONFIG_VIP_CHANGE_RACE))
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff006699 .vip changerace|r - Сменить расу");
                if (sWorld->getBoolConfig(CONFIG_VIP_CUSTOMIZE))
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff006699 .vip customize|r - Изменить внешность персонажа");
                if (sWorld->getBoolConfig(CONFIG_VIP_APPEAR))
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff006699 .vip app|r - Телепорт к участнику группы");
                player->PlayerTalkClass->SendCloseGossip();
                break;
            }
            case 14: // Character management submenu
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_DUAL_SPEC), GOSSIP_SENDER_MAIN, 12);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_RESET_TALENT), GOSSIP_SENDER_MAIN, 71, GTS(LANG_ITEM_RESET_TALENT_SURE), 0, false);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_SKILLS_WEAPON), GOSSIP_SENDER_MAIN, 72);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_MAXSKILL), GOSSIP_SENDER_MAIN, 73);
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, GTS(LANG_ITEM_BACK_TO_MAIN_MENU), GOSSIP_SENDER_MAIN, 10);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_CLOSE), GOSSIP_SENDER_MAIN, 3);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
                break;
            }
            case 15: // Character changing submenu
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_CHANGE_NAME), GOSSIP_SENDER_MAIN, 74);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_CHANGE_OF_APPEARANCE), GOSSIP_SENDER_MAIN, 75);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_CHANGE_FACTION), GOSSIP_SENDER_MAIN, 76);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_CHANGE_RACE), GOSSIP_SENDER_MAIN, 77);
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, GTS(LANG_ITEM_BACK_TO_MAIN_MENU), GOSSIP_SENDER_MAIN, 10);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, GTS(LANG_ITEM_CLOSE), GOSSIP_SENDER_MAIN, 3);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
                break;
            }
            case 16: // repair
            {
                player->DurabilityRepairAll(false, 0.0f, true);
                ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_REPAIR_CONFIRM);
                player->PlayerTalkClass->SendCloseGossip();
                break;
            }
            case 17: // Remove deserter
            {
                if (!player->HasAura(26013))
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_DESERTER_NOT_FOUND);
                }
                else
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    player->RemoveAurasDueToSpell(26013);
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_DESERTER_REMOVED);
                }
                break;
            }
            case 18: // Remove weakness
            {
                player->RemoveAura(15007);
                player->PlayerTalkClass->SendCloseGossip();
                break;
            }
            case 19: // Give buffs
            {
                if (player->HasAura(45523))
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    player->GetSession()->SendAreaTriggerMessage(GTS(LANG_ITEM_MSG_COOLDOWN));
                }
                else
                {
                    player->RemoveAurasByType(SPELL_AURA_MOUNTED);
                    for (size_t i = 0; i < aurassSize; ++i)
                        player->AddAura(aurass[i], player);
                    ChatHandler(player->GetSession()).SendNotification("|cffC67171Баффы получены!");
                    player->CastSpell(player, 45523, true);
                    player->PlayerTalkClass->SendCloseGossip();
                }
                break;
            }
            case 20: // VIP bank
            {
                player->PlayerTalkClass->SendCloseGossip();
                player->GetSession()->SendShowBank(player->GetGUID());
                break;
            }
            case 22: // Remove cooldown
            {
                if (player->HasAura(45523))
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    player->GetSession()->SendAreaTriggerMessage(GTS(LANG_ITEM_MSG_RESET_COOLDOWN));
                }
                else
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    player->RemoveArenaSpellCooldowns(true);
                    player->GetSession()->SendAreaTriggerMessage(GTS(LANG_ITEM_MSG_RESET_COOLDOWN));
                    player->CastSpell(player, 45523, true);
                }
                break;
            }
            case 25: // Restore HP/Mana
            {
                if (player->IsInCombat())
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    ChatHandler(player->GetSession()).PSendSysMessage(GTS(LANG_ITEM_ERROR_IN_COMBAT));
                    return;
                }
                else if (player->IsInFlight())
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    ChatHandler(player->GetSession()).PSendSysMessage(GTS(LANG_ITEM_ERROR_IN_FLIGHT));
                    return;
                }
                else if (player->GetMap()->IsBattlegroundOrArena())
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    ChatHandler(player->GetSession()).PSendSysMessage(GTS(LANG_ITEM_ERROR_IN_BG));
                    return;
                }
                else
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    if (player->getPowerType() == POWER_MANA)
                        player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));

                    player->SetHealth(player->GetMaxHealth());
                    player->CastSpell(player, 31726, true);
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_MSG_RESTORE_HP_MANA);
                }
                break;
            }
            case 26: // Reset saved instances
            {
                if (player->IsInCombat())
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    ChatHandler(player->GetSession()).PSendSysMessage(GTS(LANG_ITEM_ERROR_IN_COMBAT));
                    return;
                }
                else if (player->IsInFlight())
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    ChatHandler(player->GetSession()).PSendSysMessage(GTS(LANG_ITEM_ERROR_IN_FLIGHT));
                    return;
                }
                else if (player->GetMap()->IsBattlegroundOrArena())
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    ChatHandler(player->GetSession()).PSendSysMessage(GTS(LANG_ITEM_ERROR_IN_BG));
                    return;
                }
                else
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    // Удаляем все сохраненные подземелья, кроме текущей карты
                    for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
                    {
                        BoundInstancesMap const& m_boundInstances = sInstanceSaveMgr->PlayerGetBoundInstances(player->GetGUID(), Difficulty(i));
                        for (BoundInstancesMap::const_iterator itr = m_boundInstances.begin(); itr != m_boundInstances.end();)
                        {
                            if (itr->first != player->GetMapId())
                            {
                                sInstanceSaveMgr->PlayerUnbindInstance(player->GetGUID(), itr->first, Difficulty(i), true, player);
                                itr = m_boundInstances.begin();
                            }
                            else
                                ++itr;
                        }
                    }
                    player->CastSpell(player, 59908, true);
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_MSG_RESET_SAVED_INSTANCES);
                }
                break;
            }
            case 71: // Reset talents
            {
                player->resetTalents(true);
                player->SendTalentsInfoData(false);
                CloseGossipMenuFor(player);
                ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_TALENT_RESET_CONFIRM);
                break;
            }
            case 72: // Weapon skills
            {
                switch (player->getClass())
                {
                case CLASS_WARRIOR:
                    CloseGossipMenuFor(player);
                    player->learnSpell(5011, false); // Crossbows
                    player->learnSpell(200, false); // Polearms
                    player->learnSpell(15590, false);
                    player->learnSpell(264, false);
                    player->learnSpell(266, false);
                    player->learnSpell(227, false);
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_SKILLS_WEAPON_CONFIRM);
                    break;
                case CLASS_PALADIN:
                    CloseGossipMenuFor(player);
                    player->learnSpell(197, false); // Two-handed axes
                    player->learnSpell(200, false); // Polearms
                    player->learnSpell(196, false); // One - handed axes
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_SKILLS_WEAPON_CONFIRM);
                    break;
                case CLASS_WARLOCK:
                    CloseGossipMenuFor(player);
                    player->learnSpell(201, false); // One - handed sword
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_SKILLS_WEAPON_CONFIRM);
                    break;
                case CLASS_PRIEST:
                    CloseGossipMenuFor(player);
                    player->learnSpell(1180, false);
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_SKILLS_WEAPON_CONFIRM);
                    break;
                case CLASS_HUNTER:
                    CloseGossipMenuFor(player);
                    player->learnSpell(5011, false); // Crossbows
                    player->learnSpell(202, false); // Dual - handed sword
                    player->learnSpell(200, false); // Polearms
                    player->learnSpell(15590, false);
                    player->learnSpell(264, false);
                    player->learnSpell(2567, false);
                    player->learnSpell(227, false);
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_SKILLS_WEAPON_CONFIRM);
                    break;
                case CLASS_MAGE:
                    CloseGossipMenuFor(player);
                    player->learnSpell(1180, false);
                    player->learnSpell(201, false); // One - handed sword
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_SKILLS_WEAPON_CONFIRM);
                    break;
                case CLASS_SHAMAN:
                    CloseGossipMenuFor(player);
                    player->learnSpell(199, false);
                    player->learnSpell(197, false); // Two-handed axes
                    player->learnSpell(1180, false);
                    player->learnSpell(15590, false);
                    player->learnSpell(196, false); // One - handed axes
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_SKILLS_WEAPON_CONFIRM);
                    break;
                case CLASS_ROGUE:
                    CloseGossipMenuFor(player);
                    player->learnSpell(5011, false); // Crossbows
                    player->learnSpell(198, false);
                    player->learnSpell(15590, false);
                    player->learnSpell(264, false);
                    player->learnSpell(201, false); // One - handed sword
                    player->learnSpell(266, false);
                    player->learnSpell(196, false); // One - handed axes
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_SKILLS_WEAPON_CONFIRM);
                    break;
                case CLASS_DEATH_KNIGHT:
                    CloseGossipMenuFor(player);
                    player->learnSpell(199, false);
                    player->learnSpell(198, false);
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_SKILLS_WEAPON_CONFIRM);
                    break;
                case CLASS_DRUID:
                    CloseGossipMenuFor(player);
                    player->learnSpell(199, false);
                    player->learnSpell(200, false); // Polearms
                    player->learnSpell(15590, false);
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_SKILLS_WEAPON_CONFIRM);
                    break;
                }
                break;
            }
            case 73: // Max skills
            {
                CloseGossipMenuFor(player);
                player->UpdateSkillsToMaxSkillsForLevel();
                ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_MAXSKILL_CONFIRM);
                break;
            }
            case 74: // Change name
            {
                if (player->GetArenaPoints() < CONST_ARENA_RENAME)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_MSG_NO_ARENA_POINTS);
                    player->PlayerTalkClass->SendCloseGossip();
                }
                else
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    player->SetAtLoginFlag(AT_LOGIN_RENAME);
                    player->ModifyArenaPoints(-CONST_ARENA_RENAME);
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_MSG_RENAME_COMPLETE);
                }
                break;
            }
            case 75: // Customize
            {
                if (player->GetArenaPoints() < CONST_ARENA_CUSTOMIZE)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_MSG_NO_ARENA_POINTS);
                    player->PlayerTalkClass->SendCloseGossip();
                }
                else
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
                    player->ModifyArenaPoints(-CONST_ARENA_CUSTOMIZE);
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_MSG_CUSTOMIZE_COMPLETE);
                }
                break;
            }
            case 76: // Change Faction
            {
                if (player->GetArenaPoints() < CONST_ARENA_CHANGE_FACTION)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_MSG_NO_ARENA_POINTS);
                    player->PlayerTalkClass->SendCloseGossip();
                }
                else
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
                    player->ModifyArenaPoints(-CONST_ARENA_CHANGE_FACTION);
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_MSG_CHANGE_FACTION_COMPLETE);
                }
                break;
            }
            case 77: // Change Race
            {
                if (player->GetArenaPoints() < CONST_ARENA_CHANGE_RACE)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_MSG_NO_ARENA_POINTS);
                    player->PlayerTalkClass->SendCloseGossip();
                }
                else
                {
                    player->PlayerTalkClass->SendCloseGossip();
                    player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
                    player->ModifyArenaPoints(-CONST_ARENA_CHANGE_RACE);
                    ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_MSG_CHANGE_RACE_COMPLETE);
                }
                break;
            }
            }
        }
        else if (sender == GOSSIP_SENDER_MAIN + 4) // Teleportation main categories
        {
            uint8 faction = player->GetTeamId() == TEAM_ALLIANCE ? 2 : 1; // 1 = Орда, 2 = Альянс
            sCustomTeleportMgr->GetTeleportListAfter(player, action, faction, item->GetGUID());
        }
        else if (sender == GOSSIP_SENDER_MAIN + 5) // Teleportation execute
        {
            sCustomTeleportMgr->TeleportFunction(player, action);
        }
    }

    void OnGossipSelectCode(Player* player, Item* /*item*/, uint32 /*sender*/, uint32 action, const char* code) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (!*code)
            return;

        // For GuildWars system
        std::string guildName = code;

        Guild* targetGuild = sGuildMgr->GetGuildByName(guildName);
        if (!targetGuild)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(LANG_GUILD_NOT_FOUND, guildName);
            player->PlayerTalkClass->SendCloseGossip();
            return;
        }

        Guild* ownGuild = player->GetGuild();
        if (!ownGuild)
        {
            player->PlayerTalkClass->SendCloseGossip();
            return;
        }

        player->PlayerTalkClass->SendCloseGossip();
    }
};

void AddSC_custom_item()
{
    new custom_item();
}
