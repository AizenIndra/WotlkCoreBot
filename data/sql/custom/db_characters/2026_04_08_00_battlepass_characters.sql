CREATE TABLE IF NOT EXISTS `character_battlepass_task_progress` (
  `guid` INT UNSIGNED NOT NULL,
  `season_id` INT UNSIGNED NOT NULL,
  `category` VARCHAR(8) NOT NULL,
  `task_id` INT UNSIGNED NOT NULL,
  `current_progress` INT UNSIGNED NOT NULL DEFAULT 0,
  `required_progress` INT UNSIGNED NOT NULL DEFAULT 0,
  `claimed` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `updated_at` INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`, `season_id`, `category`, `task_id`),
  KEY `idx_bp_char_task_lookup` (`guid`, `category`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
