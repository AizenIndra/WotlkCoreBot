-- Guild Level System - world DB changes (custom)
-- Tables for guild perks, XP per level, and creature kill rewards

CREATE TABLE IF NOT EXISTS `guild_bonus_spell` (
  `level` TINYINT(3) UNSIGNED NOT NULL,
  `spell` INT(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`level`, `spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `guild_xp_for_next_level` (
  `level` TINYINT(3) UNSIGNED NOT NULL,
  `xp_for_next_level` INT(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`level`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `creature_onkill_reward_guildxp` (
  `creatureID` INT(10) UNSIGNED NOT NULL,
  `rewardXP` INT(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`creatureID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

