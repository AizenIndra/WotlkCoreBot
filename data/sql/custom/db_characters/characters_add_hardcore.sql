-- Hardcore opt-in: store player choice at character creation
ALTER TABLE `characters` ADD COLUMN `hardcore` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '1 = hardcore' AFTER `extraBonusTalentCount`;
