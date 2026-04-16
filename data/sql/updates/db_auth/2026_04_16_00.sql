-- DB update 2025_07_24_00 -> 2026_04_16_00
-- Optional anticheat log table (AccountMgr::RecordAntiCheatLog / LOGIN_INS_ACCOUNT_ANTICHEAT)

CREATE TABLE IF NOT EXISTS `anticheat_logs` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `times` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `account` int NOT NULL,
  `player` varchar(96) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `description` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `position` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `realmId` int NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
