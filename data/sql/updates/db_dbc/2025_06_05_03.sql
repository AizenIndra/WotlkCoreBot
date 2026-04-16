-- Split from db_world/2025_06_05_03.sql: DBC-only statements (acore_dbc)

-- Note: Packs of 4 hunter/lancer/fire weaver/lancer seem to always social aggro, source looks similar

-- Spelldifficulty
DELETE FROM `spelldifficulty_dbc` WHERE `ID` IN(55700,55703,55602,55603,55659,55613,55622,55635,55636,16172,35946,55624,55599,55597,55530,55663,55348,55521);

INSERT INTO `spelldifficulty_dbc` (`ID`, `DifficultySpellID_1`, `DifficultySpellID_2`, `DifficultySpellID_3`, `DifficultySpellID_4`) VALUES
(55700, 55700, 59019, 0, 0),
(55703, 55703, 59020, 0, 0),
(55602, 55602, 59021, 0, 0),
(55603, 55603, 59022, 0, 0),
(55659, 55659, 58972, 0, 0),
(55613, 55613, 58971, 0, 0),
(55622, 55622, 58978, 0, 0),
(55635, 55635, 58975, 0, 0),
(55636, 55636, 58977, 0, 0),
(16172, 16172, 58969, 0, 0),
(35946, 35946, 59146, 0, 0),
(55624, 55624, 58973, 0, 0),
(55599, 55599, 58981, 0, 0),
(55597, 55597, 58980, 0, 0),
(55530, 55530, 58991, 0, 0),
(55663, 55663, 58992, 0, 0),
(55348, 55348, 58966, 0, 0),
(55521, 55521, 58967, 0, 0);
