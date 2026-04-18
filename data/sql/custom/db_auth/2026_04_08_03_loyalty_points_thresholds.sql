SET @col_exists := (
  SELECT COUNT(*)
  FROM information_schema.COLUMNS
  WHERE TABLE_SCHEMA = DATABASE()
    AND TABLE_NAME = 'account_donate'
    AND COLUMN_NAME = 'loyalty_points'
);

SET @sql := IF(
  @col_exists = 0,
  'ALTER TABLE `account_donate` ADD COLUMN `loyalty_points` INT UNSIGNED NOT NULL DEFAULT 0 AFTER `loyalty_level`',
  'SELECT 1'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

CREATE TABLE IF NOT EXISTS `custom_store_loyalty_level` (
  `level` INT UNSIGNED NOT NULL,
  `required_points` INT UNSIGNED NOT NULL,
  PRIMARY KEY (`level`),
  UNIQUE KEY `uk_required_points` (`required_points`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

INSERT INTO `custom_store_loyalty_level` (`level`, `required_points`) VALUES
(0, 0),
(1, 100),
(2, 250),
(3, 500),
(4, 800),
(5, 1200),
(6, 1700),
(7, 2300),
(8, 3000),
(9, 3800),
(10, 4700)
ON DUPLICATE KEY UPDATE `required_points` = VALUES(`required_points`);
