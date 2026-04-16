-- Split from db_world/2025_12_29_10.sql: DBC-only statements (acore_dbc)

DELETE FROM `spelldifficulty_dbc` WHERE `ID` IN (53406, 53317, 53394, 53330, 53322);

INSERT INTO `spelldifficulty_dbc` (`ID`, `DifficultySpellID_1`, `DifficultySpellID_2`, `DifficultySpellID_3`, `DifficultySpellID_4`) VALUES
(53406, 53406, 59420, 0, 0),
(53317, 53317, 59343, 0, 0),
(53394, 53394, 59344, 0, 0),
(53330, 53330, 59348, 0, 0),
(53322, 53322, 59347, 0, 0);
