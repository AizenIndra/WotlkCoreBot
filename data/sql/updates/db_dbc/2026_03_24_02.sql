-- DB update (split from db_world 2026_03_24_02): spell difficulty for Utgarde Pinnacle / Skadi
-- Applies to database acore_dbc

-- Missing spell difficulty entries
DELETE FROM `spelldifficulty_dbc` WHERE `ID` IN (50228, 50258, 49084, 49089, 49091);
INSERT INTO `spelldifficulty_dbc` (`ID`, `DifficultySpellID_1`, `DifficultySpellID_2`, `DifficultySpellID_3`, `DifficultySpellID_4`) VALUES
(49084, 49084, 59246, 0, 0),
(49089, 49089, 59247, 0, 0),
(49091, 49091, 59249, 0, 0),
(50228, 50228, 59322, 0, 0),
(50258, 50258, 59334, 0, 0);
