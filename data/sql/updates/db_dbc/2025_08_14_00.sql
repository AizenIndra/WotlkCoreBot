-- Split from db_world/2025_08_14_00.sql: DBC-only statements (acore_dbc)

-- Fix misplaced spell IDs in spelldifficulty for Skittering Infector's Acid Splash
UPDATE `spelldifficulty_dbc` SET `DifficultySpellID_1` = 52446 WHERE `ID` = 59363;

UPDATE `spelldifficulty_dbc` SET `DifficultySpellID_2` = 59363 WHERE `ID` = 59363;
