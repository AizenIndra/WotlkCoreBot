-- Split from db_world/2025_11_03_00.sql: DBC-only statements (acore_dbc)

-- DB update 2025_11_01_03 -> 2025_11_03_00
--
DELETE FROM `spelldifficulty_dbc` WHERE `ID` IN (53618, 53616, 53617, 53602);

INSERT INTO `spelldifficulty_dbc` (`ID`, `DifficultySpellID_1`, `DifficultySpellID_2`, `DifficultySpellID_3`, `DifficultySpellID_4`) VALUES
(53618, 53618, 59350, 0, 0),
(53616, 53616, 59360, 0, 0),
(53617, 53617, 59359, 0, 0),
(53602, 53602, 59349, 0, 0);
