-- Split from db_world/2025_11_17_02.sql: DBC-only statements (acore_dbc)

-- DB update 2025_11_17_01 -> 2025_11_17_02

-- Set Spelldifficulty
DELETE FROM `spelldifficulty_dbc` WHERE (`ID` IN (52534, 52535));

INSERT INTO `spelldifficulty_dbc` (`ID`,`DifficultySpellID_1`,`DifficultySpellID_2`,`DifficultySpellID_3`,`DifficultySpellID_4`) VALUES
(52534, 52534, 59357, 0, 0),
(52535, 52535, 59358, 0, 0);
