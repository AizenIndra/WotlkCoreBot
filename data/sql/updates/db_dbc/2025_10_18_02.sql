-- Split from db_world/2025_10_18_02.sql: DBC-only statements (acore_dbc)

DELETE FROM `spelldifficulty_dbc` WHERE `ID` IN (52891, 52885, 52879, 52904);

INSERT INTO `spelldifficulty_dbc` (`ID`, `DifficultySpellID_1`, `DifficultySpellID_2`, `DifficultySpellID_3`, `DifficultySpellID_4`) VALUES
(52891, 52891, 59173, 0, 0), -- Blade Turning
(52885, 52885, 59180, 0, 0), -- Deadly Throw
(52879, 52879, 59181, 0, 0), -- Deflection
(52904, 52904, 59179, 0, 0); -- Throw
