-- Guild Level System - characters DB changes
-- Adds guild XP/level columns and member item level tracking

ALTER TABLE `guild`
  ADD COLUMN `xp` INT(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `BankMoney`,
  ADD COLUMN `level` TINYINT(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `xp`,
  ADD COLUMN `todayXP` INT(10) UNSIGNED NOT NULL DEFAULT 0 AFTER `level`;

ALTER TABLE `guild_member`
  ADD COLUMN `ItemLvl` INT(10) NOT NULL DEFAULT 0 AFTER `offnote`;

