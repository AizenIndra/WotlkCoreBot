-- DB update 2025_07_24_00 -> 2026_02_19_00
-- Add account_premium table for premium account feature
--
CREATE TABLE IF NOT EXISTS `account_premium` (
    `id` INT UNSIGNED NOT NULL COMMENT 'Account id',
    `setdate` INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Unix timestamp when premium was set',
    `unsetdate` INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Unix timestamp when premium expires',
    `active` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '1 = active, 0 = inactive',
    PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='Premium account status';