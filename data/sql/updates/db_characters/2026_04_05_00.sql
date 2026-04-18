ALTER TABLE `characters`
ADD COLUMN `rankPoints` int unsigned NOT NULL DEFAULT 0 AFTER `innTriggerId`;
