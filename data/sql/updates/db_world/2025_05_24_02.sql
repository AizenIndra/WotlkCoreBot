-- DB update 2025_05_24_01 -> 2025_05_24_02
--
DELETE FROM `spell_proc_event` WHERE `entry` IN (45396,45398);

INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `procPhase`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
(45396, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 45000),
(45398, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45000);

DELETE FROM `spell_script_names` WHERE `spell_id` IN (45396,45398) AND `ScriptName`='spell_gen_weapon_coating_enchant';

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(45396, 'spell_gen_weapon_coating_enchant'),
(45398, 'spell_gen_weapon_coating_enchant');
