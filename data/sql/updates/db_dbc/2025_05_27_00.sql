-- Split from db_world/2025_05_27_00.sql: DBC-only statements (acore_dbc)

-- DB update 2025_05_26_00 -> 2025_05_27_00
-- 30695 (Portal Keeper)
DELETE FROM `spelldifficulty_dbc` WHERE `ID` IN (58529,58532);

INSERT INTO `spelldifficulty_dbc` (`ID`, `DifficultySpellID_1`, `DifficultySpellID_2`, `DifficultySpellID_3`, `DifficultySpellID_4`) VALUES
(58529, 58529, 61592, 0, 0),
(58532, 58532, 61594, 0, 0);
