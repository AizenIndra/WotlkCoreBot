CREATE TABLE IF NOT EXISTS `battlepass_season` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `category` VARCHAR(8) NOT NULL,
  `name` VARCHAR(120) NOT NULL,
  `active` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `start_time` INT UNSIGNED NOT NULL DEFAULT 0,
  `end_time` INT UNSIGNED NOT NULL DEFAULT 0,
  `premium_cost` INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`),
  KEY `idx_bp_season_cat_active` (`category`, `active`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `battlepass_reward` (
  `season_id` INT UNSIGNED NOT NULL,
  `tier` INT UNSIGNED NOT NULL,
  `free_item` INT UNSIGNED NOT NULL DEFAULT 0,
  `free_count` INT UNSIGNED NOT NULL DEFAULT 1,
  `premium_item` INT UNSIGNED NOT NULL DEFAULT 0,
  `premium_count` INT UNSIGNED NOT NULL DEFAULT 1,
  `xp_req` INT UNSIGNED NOT NULL DEFAULT 0,
  `reward_type` TINYINT UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (`season_id`, `tier`),
  KEY `idx_bp_reward_xp` (`season_id`, `xp_req`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `battlepass_task` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `season_id` INT UNSIGNED NOT NULL,
  `category` VARCHAR(8) NOT NULL,
  `task_type` VARCHAR(32) NOT NULL,
  `event_type` VARCHAR(32) NOT NULL DEFAULT 'QUEST_COMPLETE',
  `target_id` INT UNSIGNED NOT NULL DEFAULT 0,
  `title` VARCHAR(255) NOT NULL,
  `description` TEXT NULL,
  `requirement` INT UNSIGNED NOT NULL DEFAULT 0,
  `reward_xp` INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`),
  KEY `idx_bp_task_lookup` (`season_id`, `category`, `task_type`),
  KEY `idx_bp_task_event` (`season_id`, `category`, `event_type`, `target_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `battlepass_bundle` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `season_id` INT UNSIGNED NOT NULL,
  `category` VARCHAR(8) NOT NULL,
  `name` VARCHAR(120) NOT NULL,
  `cost` INT UNSIGNED NOT NULL DEFAULT 0,
  `xp` INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`),
  KEY `idx_bp_bundle_lookup` (`season_id`, `category`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
