-- CharSections.dbc (also used by mod-playerbots; must live in acore_dbc)

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;

DROP TABLE IF EXISTS `charsections_dbc`;
CREATE TABLE IF NOT EXISTS `charsections_dbc` (
  `ID` int NOT NULL DEFAULT '0',
  `Race` int NOT NULL DEFAULT '0',
  `Gender` int NOT NULL DEFAULT '0',
  `GenType` int NOT NULL DEFAULT '0',
  `TexturePath1` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `TexturePath2` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `TexturePath3` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `Flags` int NOT NULL DEFAULT '0',
  `Type` int NOT NULL DEFAULT '0',
  `Color` int NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

DELETE FROM `charsections_dbc`;

/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
