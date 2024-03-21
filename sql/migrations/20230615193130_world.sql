SET NAMES utf8;
DROP PROCEDURE IF EXISTS add_migration;
delimiter ??
CREATE PROCEDURE `add_migration`()
BEGIN
DECLARE v INT DEFAULT 1;
SET v = (SELECT COUNT(*) FROM `migrations` WHERE `id`='20230615193130');
IF v=0 THEN
INSERT INTO `migrations` VALUES ('20230615193130');
-- Add your query below.


-- Mr. Bigglesworth does random movement at a few different points.
UPDATE `creature` SET `position_x`=3007.71, `position_y`=-3432.56, `position_z`=293.975, `orientation`=1.63228 WHERE `id`=16998;
DELETE FROM `creature_movement` WHERE `id`=300362;
INSERT INTO `creature_movement` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `waittime`, `wander_distance`, `script_id`) VALUES
(300362, 1, 3006.61, -3414.67, 297.135, 100, 0, 0, 0),
(300362, 2, 3005.11, -3394.36, 298.357, 100, 30000, 5, 0),
(300362, 3, 2996.11, -3395.27, 298.357, 100, 0, 0, 0),
(300362, 4, 2977.22, -3405.25, 298.296, 100, 0, 0, 0),
(300362, 5, 2969.01, -3418.7, 298.357, 100, 0, 0, 0),
(300362, 6, 2960.27, -3433.89, 299.557, 100, 30000, 5, 0),
(300362, 7, 2967.05, -3442.96, 298.357, 100, 0, 0, 0),
(300362, 8, 2983.29, -3455.8, 298.333, 100, 0, 0, 0),
(300362, 9, 2991.66, -3448.95, 302.017, 100, 0, 0, 0),
(300362, 10, 2997.92, -3442.53, 304.357, 100, 0, 0, 0),
(300362, 11, 3005.89, -3434.28, 304.415, 100, 30000, 5, 0),
(300362, 12, 3016.62, -3445.3, 303.046, 100, 0, 0, 0),
(300362, 13, 3026.83, -3455.46, 298.357, 100, 0, 0, 0),
(300362, 14, 3037.66, -3452.39, 298.357, 100, 0, 0, 0),
(300362, 15, 3040.89, -3433.78, 298.357, 100, 30000, 5, 0),
(300362, 16, 3006.79, -3434.28, 293.957, 100, 30000, 5, 0);


-- End of migration.
END IF;
END??
delimiter ; 
CALL add_migration();
DROP PROCEDURE IF EXISTS add_migration;
