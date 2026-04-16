-- Add CharSections / EmotesTextSound mirror tables if missing (were previously only in mod-playerbots data/sql/world/base)

/*!40101 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;

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

CREATE TABLE IF NOT EXISTS `emotetextsound_dbc` (
  `ID` int NOT NULL DEFAULT '0',
  `EmotesTextId` int NOT NULL DEFAULT '0',
  `RaceId` int NOT NULL DEFAULT '0',
  `SexId` int NOT NULL DEFAULT '0',
  `SoundId` int NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
