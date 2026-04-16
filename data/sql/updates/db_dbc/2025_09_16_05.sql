-- Split from db_world/2025_09_16_05.sql: DBC-only statements (acore_dbc)

DELETE FROM `spelldifficulty_dbc` WHERE `ID` IN (54315, 54314, 54309);

INSERT INTO `spelldifficulty_dbc` (`ID`, `DifficultySpellID_1`, `DifficultySpellID_2`) VALUES
(54315, 54315, 59355),
(54314, 54314, 59354),
(54309, 54309, 59352);
