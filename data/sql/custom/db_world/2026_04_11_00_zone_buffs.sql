-- Zone-specific auras (custom: PlayerScript buff_zone in orrig.cpp)
-- zone_id: DBC zone id (AreaTable / Zone)
-- buff_id: spell id applied while the player is in that zone

CREATE TABLE IF NOT EXISTS `zone_buffs` (
  `zone_id` INT UNSIGNED NOT NULL,
  `buff_id` INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`zone_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
