-- Split from db_world/2025_11_01_02.sql: DBC-only statements (acore_dbc)

-- DB update 2025_11_01_01 -> 2025_11_01_02

-- Spelldifficulty
DELETE FROM `spelldifficulty_dbc` WHERE `ID` IN (50378);

INSERT INTO `spelldifficulty_dbc` (`ID`,`DifficultySpellID_1`,`DifficultySpellID_2`,`DifficultySpellID_3`,`DifficultySpellID_4`) VALUES
(50378, 50378, 59017, 0, 0);
