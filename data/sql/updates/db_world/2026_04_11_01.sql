-- DB update 2026_04_11_01
-- Custom: zone_buffs for PlayerScript buff_zone (scripts/Custom/Boss/worldbosses/orrig.cpp)

CREATE TABLE IF NOT EXISTS `zone_buffs` (
  `zone_id` INT UNSIGNED NOT NULL,
  `buff_id` INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`zone_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
