-- Guild applications storage

CREATE TABLE IF NOT EXISTS `guild_applications` (
  `id`         INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `guildId`    INT UNSIGNED NOT NULL,
  `playerGuid` INT UNSIGNED NOT NULL,
  `note`       VARCHAR(255) NOT NULL DEFAULT '',
  `createTime` INT UNSIGNED NOT NULL DEFAULT 0,
  `status`     TINYINT UNSIGNED NOT NULL DEFAULT 0, -- 0=pending,1=accepted,2=rejected
  PRIMARY KEY (`id`),
  KEY `idx_guild` (`guildId`),
  KEY `idx_player_status` (`playerGuid`, `status`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

