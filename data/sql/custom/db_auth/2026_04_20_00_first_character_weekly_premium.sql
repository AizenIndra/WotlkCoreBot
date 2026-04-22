CREATE TABLE IF NOT EXISTS `account_first_character_premium` (
  `account_id` INT UNSIGNED NOT NULL,
  `account_name` VARCHAR(32) NOT NULL,
  `character_guid` INT UNSIGNED NOT NULL,
  `character_name` VARCHAR(12) NOT NULL,
  `granted_at` INT UNSIGNED NOT NULL DEFAULT 0,
  `premium_unsetdate` INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`account_id`),
  KEY `idx_character_guid` (`character_guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci
COMMENT='One-time premium grant log for first character per account';
