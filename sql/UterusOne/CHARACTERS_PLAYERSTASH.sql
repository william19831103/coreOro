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

CREATE TABLE IF NOT EXISTS `player_stash_buffs` (
  `char_guid` int(11) unsigned NOT NULL DEFAULT 0,
  `temp_id` int(11) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `spell_id` int(11) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `gossip_text` varchar(100) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

CREATE TABLE IF NOT EXISTS `player_stash_gear` (
  `char_guid` int(11) unsigned NOT NULL,
  `temp_id` int(10) unsigned NOT NULL DEFAULT 0,
  `gossip_text` varchar(100) NOT NULL,
  `item_slot` int(10) unsigned NOT NULL DEFAULT 0,
  `item_entry` int(10) unsigned NOT NULL DEFAULT 0,
  `item_enchant` int(10) unsigned NOT NULL DEFAULT 0,
  `patch` tinyint(3) unsigned NOT NULL DEFAULT 0 COMMENT 'Content patch in which this exact version of the entry was added'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Templates';

CREATE TABLE IF NOT EXISTS `player_stash_talents` (
  `char_guid` int(11) unsigned NOT NULL DEFAULT 0,
  `temp_id` int(11) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `gossip_text` varchar(100) NOT NULL,
  `talent_id` int(11) unsigned NOT NULL DEFAULT 0 COMMENT 'Spell Identifier',
  `rank` int(11) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
