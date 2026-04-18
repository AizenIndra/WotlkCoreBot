CREATE TABLE IF NOT EXISTS `account_battlepass_progress` (
  `account_id` INT UNSIGNED NOT NULL,
  `season_id` INT UNSIGNED NOT NULL,
  `category` VARCHAR(8) NOT NULL,
  `total_xp` INT UNSIGNED NOT NULL DEFAULT 0,
  `premium_status` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `updated_at` INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`account_id`, `season_id`, `category`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `account_battlepass_reward_claim` (
  `account_id` INT UNSIGNED NOT NULL,
  `season_id` INT UNSIGNED NOT NULL,
  `category` VARCHAR(8) NOT NULL,
  `tier` INT UNSIGNED NOT NULL,
  `free_claimed` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `premium_claimed` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `updated_at` INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`account_id`, `season_id`, `category`, `tier`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `account_battlepass_reroll` (
  `account_id` INT UNSIGNED NOT NULL,
  `season_id` INT UNSIGNED NOT NULL,
  `category` VARCHAR(8) NOT NULL,
  `reroll_count` INT UNSIGNED NOT NULL DEFAULT 0,
  `next_cost_copper` INT UNSIGNED NOT NULL DEFAULT 10000,
  `updated_at` INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`account_id`, `season_id`, `category`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
