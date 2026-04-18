-- Guild reputation rewards configuration
-- Used by AddonIO::HandleGuildGetReputationReward
-- Table structure: itemEntry (item to give), minGuildLevel, costGold (copper)

CREATE TABLE IF NOT EXISTS `guild_reputation_rewards` (
  `itemEntry` INT(10) UNSIGNED NOT NULL,
  `minGuildLevel` TINYINT(3) UNSIGNED NOT NULL DEFAULT 0,
  `costGold` INT(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`itemEntry`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Example (commented out):
-- INSERT INTO `guild_reputation_rewards` (`itemEntry`, `minGuildLevel`, `costGold`) VALUES
-- (12345, 5, 100000); -- item 12345, requires guild level 5, costs 10 gold

