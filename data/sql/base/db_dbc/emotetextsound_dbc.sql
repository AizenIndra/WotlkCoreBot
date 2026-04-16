-- EmotesTextSound.dbc (also used by mod-playerbots; must live in acore_dbc)

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;

DROP TABLE IF EXISTS `emotetextsound_dbc`;
CREATE TABLE IF NOT EXISTS `emotetextsound_dbc` (
  `ID` int NOT NULL DEFAULT '0',
  `EmotesTextId` int NOT NULL DEFAULT '0',
  `RaceId` int NOT NULL DEFAULT '0',
  `SexId` int NOT NULL DEFAULT '0',
  `SoundId` int NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

DELETE FROM `emotetextsound_dbc`;

/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
