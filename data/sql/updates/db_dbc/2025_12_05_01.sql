-- Split from db_world/2025_12_05_01.sql: DBC-only statements (acore_dbc)

-- Add Spell Difficulty (Devotion Aura)
DELETE FROM `spelldifficulty_dbc` WHERE (`ID` IN (57740));

INSERT INTO `spelldifficulty_dbc` (`ID`,`DifficultySpellID_1`,`DifficultySpellID_2`,`DifficultySpellID_3`,`DifficultySpellID_4`) VALUES
(57740, 57740, 58944, 0, 0);
