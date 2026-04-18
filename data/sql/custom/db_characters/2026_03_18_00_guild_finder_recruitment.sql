-- Guild Finder / LookingForGuild recruitment settings

CREATE TABLE IF NOT EXISTS `guild_finder_recruitment` (
  `guildId` INT UNSIGNED NOT NULL,
  `listed` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `classRoles` INT UNSIGNED NOT NULL DEFAULT 0,
  `availability` INT UNSIGNED NOT NULL DEFAULT 0,
  `interests` INT UNSIGNED NOT NULL DEFAULT 0,
  `levelType` TINYINT UNSIGNED NOT NULL DEFAULT 1,
  `comment` VARCHAR(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`guildId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

