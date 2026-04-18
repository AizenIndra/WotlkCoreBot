-- DB update: bind rank experience items to ItemUse_Glory_Exp script

UPDATE `item_template`
SET `ScriptName` = 'ItemUse_Glory_Exp'
WHERE `entry` IN (1042, 1043, 1044, 35778, 842);
