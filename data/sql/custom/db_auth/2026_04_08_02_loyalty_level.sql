SET @col_exists := (
  SELECT COUNT(*)
  FROM information_schema.COLUMNS
  WHERE TABLE_SCHEMA = DATABASE()
    AND TABLE_NAME = 'account_donate'
    AND COLUMN_NAME = 'loyalty_level'
);

SET @sql := IF(
  @col_exists = 0,
  'ALTER TABLE `account_donate` ADD COLUMN `loyalty_level` INT UNSIGNED NOT NULL DEFAULT 0 AFTER `votes`',
  'SELECT 1'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;
