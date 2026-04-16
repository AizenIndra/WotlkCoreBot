-- Split from db_world/2025_12_29_02.sql: DBC-only statements (acore_dbc)

-- DB update 2025_12_29_01 -> 2025_12_29_02
--
DELETE FROM `spelldifficulty_dbc` WHERE `ID`=54095;

INSERT INTO `spelldifficulty_dbc` (`ID`, `DifficultySpellID_1`, `DifficultySpellID_2`, `DifficultySpellID_3`, `DifficultySpellID_4`) VALUES
(54095, 54095, 54096, 0, 0);
