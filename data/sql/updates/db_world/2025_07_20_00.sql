DELETE FROM `spell_script_names` WHERE `spell_id` = 67557;

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(67557, 'spell_pet_intellect_spirit_resilience_scaling');

UPDATE `creature_template_addon` SET `auras` = '' WHERE (`entry` = 27829);
