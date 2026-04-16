-- Split from db_world/2026_02_16_00.sql: DBC-only statements (acore_dbc)

DELETE FROM `spelldifficulty_dbc` WHERE `ID`=57733;

INSERT INTO `spelldifficulty_dbc` (`ID`, `DifficultySpellID_1`, `DifficultySpellID_2`, `DifficultySpellID_3`, `DifficultySpellID_4`) VALUES
(57733, 57733, 58942, 0, 0);

DELETE FROM `spelldifficulty_dbc` WHERE `ID` IN (58948, 57728);

INSERT INTO `spelldifficulty_dbc` (`ID`, `DifficultySpellID_1`, `DifficultySpellID_2`, `DifficultySpellID_3`, `DifficultySpellID_4`) VALUES
(58948, 58948, 39647, 0, 0),
(57728, 57728, 58947, 0, 0);

DELETE FROM `spelldifficulty_dbc` WHERE `ID`=57750;

INSERT INTO `spelldifficulty_dbc` (`ID`, `DifficultySpellID_1`, `DifficultySpellID_2`, `DifficultySpellID_3`, `DifficultySpellID_4`) VALUES
(57750, 57750, 58037, 0, 0);
