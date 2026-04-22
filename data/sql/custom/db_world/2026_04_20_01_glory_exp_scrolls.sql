-- Bind glory exp items to ItemUse_Glory_Exp and normalize names.

UPDATE `item_template`
SET `ScriptName` = 'ItemUse_Glory_Exp',
    `name` = 'Experience Scroll'
WHERE `entry` IN (842, 1042, 1043, 1044, 35778);

DELETE FROM `item_template_locale`
WHERE `ID` IN (842, 1042, 1043, 1044, 35778)
  AND `locale` = 'ruRU';

INSERT INTO `item_template_locale` (`ID`, `locale`, `Name`, `Description`, `VerifiedBuild`) VALUES
(842, 'ruRU', 'Свиток опыта', '', -12340),
(1042, 'ruRU', 'Свиток опыта', '', -12340),
(1043, 'ruRU', 'Свиток опыта', '', -12340),
(1044, 'ruRU', 'Свиток опыта', '', -12340),
(35778, 'ruRU', 'Свиток опыта', '', -12340);
