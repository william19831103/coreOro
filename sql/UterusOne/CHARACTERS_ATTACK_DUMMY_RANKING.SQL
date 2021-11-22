/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

CREATE DATABASE IF NOT EXISTS `characters` /*!40100 DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci */;
USE `characters`;

CREATE TABLE IF NOT EXISTS `attack_dummy_ranking` (
  `guid` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `character_guid` int(10) unsigned NOT NULL,
  `name` varchar(50) NOT NULL,
  `class` tinyint(4) unsigned NOT NULL DEFAULT 0,
  `gender` tinyint(4) unsigned NOT NULL DEFAULT 0,
  `race` tinyint(4) unsigned NOT NULL,
  `spec` varchar(50) NOT NULL,
  `item_level` tinyint(4) unsigned NOT NULL,
  `dps` int(11) unsigned NOT NULL,
  `damage_done` int(11) unsigned NOT NULL,
  `equipment_used` longtext DEFAULT NULL,
  `talents_used` longtext DEFAULT NULL,
  `auras_used` longtext DEFAULT NULL,
  `patch` tinyint(4) unsigned NOT NULL,
  `vaelmode` tinyint(1) DEFAULT 0,
  `mode` int(11) DEFAULT NULL,
  `time` int(11) NOT NULL DEFAULT 0,
  `date` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`guid`)
) ENGINE=MyISAM AUTO_INCREMENT=6458 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='DPS Ranking System';

/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
