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
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// This is where scripts' loading functions should be declared:
void AddSC_mod_hardcore();
void AddSC_Login_script();
void AddSC_NPC_RANK_VENDOR();
void AddSC_OnlineRewardsPlayer();
void AddSC_BattlePassProgressScripts();
void AddSC_DuelReset();

// The name of this function should match:
// void Add${NameOfDirectory}Scripts()
void AddCustomScripts()
{
    AddSC_mod_hardcore();
    AddSC_Login_script();
    AddSC_NPC_RANK_VENDOR();
    AddSC_OnlineRewardsPlayer();
    AddSC_BattlePassProgressScripts();
    AddSC_DuelReset();
}
