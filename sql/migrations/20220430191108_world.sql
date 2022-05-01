DROP PROCEDURE IF EXISTS add_migration;
delimiter ??
CREATE PROCEDURE `add_migration`()
BEGIN
DECLARE v INT DEFAULT 1;
SET v = (SELECT COUNT(*) FROM `migrations` WHERE `id`='20220430191108');
IF v=0 THEN
INSERT INTO `migrations` VALUES ('20220430191108');
-- Add your query below.


UPDATE `gameobject_template` SET `script_name`='go_murkdeep_bonfire' WHERE `entry`=61927;
UPDATE `creature_template` SET `ai_name`='EventAI', `movement_type`='1', `script_name`='' WHERE `entry` IN (2202, 2205, 2206, 10323);

-- Events list for Greymist Coastrunner
DELETE FROM `creature_ai_events` WHERE `creature_id`=2202;
INSERT INTO `creature_ai_events` (`id`, `creature_id`, `condition_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_script`, `action2_script`, `action3_script`, `comment`) VALUES (220201, 2202, 0, 2, 0, 100, 0, 15, 0, 0, 0, 220201, 0, 0, 'Greymist Coastrunner - Flee at 15% HP');
INSERT INTO `creature_ai_events` (`id`, `creature_id`, `condition_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_script`, `action2_script`, `action3_script`, `comment`) VALUES (220202, 2202, 0, 29, 0, 100, 0, 8, 1, 0, 0, 220202, 0, 0, 'Greymist Coastrunner - Reached Camp');

DELETE FROM `creature_ai_scripts` WHERE `id`=220202;
INSERT INTO `creature_ai_scripts` (`id`, `delay`, `priority`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `target_param1`, `target_param2`, `target_type`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `condition_id`, `comments`) VALUES
(220202, 0, 0, 20, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 'Greymist Coastrunner - Start Random Movement'),
(220202, 0, 0, 34, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Greymist Coastrunner - Set Home Position');

-- Waypoints for Greymist Coastrunner 1
INSERT INTO `creature_movement_special` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `waittime`, `wander_distance`, `script_id`) VALUES
(220201, 1, 4987.94, 573.735, 2.45461, 100, 0, 0, 0),
(220201, 2, 4985.97, 549.48, 5.15588, 100, 0, 0, 0),
(220201, 3, 4987.76, 547.894, 5.15979, 100, 0, 0, 0),
(220201, 4, 4987.93, 545.078, 5.31286, 100, 0, 0, 0),
(220201, 5, 4987.79, 550.355, 4.90979, 100, 0, 0, 0),
(220201, 6, 4973.62, 548.253, 6.55499, 100, 0, 0, 0);

-- Waypoints for Greymist Coastrunner 2
INSERT INTO `creature_movement_special` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `waittime`, `wander_distance`, `script_id`) VALUES
(220202, 1, 4986.87, 582.758, 0.99074, 100, 0, 0, 0),
(220202, 2, 4985.17, 560.964, 4.28174, 100, 0, 0, 0),
(220202, 3, 4989.34, 513.98, 10.2049, 100, 0, 0, 0),
(220202, 4, 4988.05, 552.285, 4.90979, 100, 0, 0, 0),
(220202, 5, 4987.43, 551.193, 4.85571, 100, 0, 0, 0),
(220202, 6, 4990.92, 552.652, 4.77905, 100, 0, 0, 0),
(220202, 7, 4987.5, 550, 4.97229, 100, 0, 0, 0),
(220202, 8, 4989.67, 552.177, 4.83667, 100, 0, 0, 0),
(220202, 9, 4973.62, 548.596, 6.53479, 100, 0, 0, 0),
(220202, 10, 4973.58, 548.616, 6.50729, 100, 0, 0, 0);

-- Waypoints for Greymist Coastrunner 3
INSERT INTO `creature_movement_special` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `waittime`, `wander_distance`, `script_id`) VALUES
(220203, 1, 4987.99, 574.97, 2.30654, 100, 0, 0, 0),
(220203, 2, 4987.33, 567.454, 3.55153, 100, 0, 0, 0),
(220203, 3, 4987.33, 566.213, 3.65979, 100, 0, 0, 0),
(220203, 4, 4986.85, 560.256, 4.24157, 100, 0, 0, 0),
(220203, 5, 4988.3, 567.397, 3.46523, 100, 0, 0, 0),
(220203, 6, 4987.78, 564, 3.91723, 100, 0, 0, 0);

-- Waypoints for Greymist Warrior 1
INSERT INTO `creature_movement_special` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `waittime`, `wander_distance`, `script_id`) VALUES
(220501, 1, 5008.56, 611.702, -1.71525, 100, 0, 0, 0),
(220501, 2, 4991.41, 583.981, 0.725115, 100, 0, 0, 0),
(220501, 3, 4979.94, 560.016, 4.39538, 100, 0, 0, 0),
(220501, 4, 5010.15, 559.19, 4.18657, 100, 0, 0, 0),
(220501, 5, 4998.22, 563.154, 4.03479, 100, 0, 0, 0);

-- Waypoints for Greymist Warrior 2
INSERT INTO `creature_movement_special` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `waittime`, `wander_distance`, `script_id`) VALUES
(220502, 1, 4991.99, 590.862, -0.100569, 100, 0, 0, 0),
(220502, 2, 4994.22, 568.495, 3.33327, 100, 0, 0, 0),
(220502, 3, 4992.22, 548.325, 5.15979, 100, 0, 0, 0),
(220502, 4, 4995.23, 563.231, 4.00964, 100, 0, 0, 0);

-- Waypoints for Greymist Hunter
INSERT INTO `creature_movement_special` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `waittime`, `wander_distance`, `script_id`) VALUES
(220601, 1, 4991.41, 604.841, -1.72439, 100, 0, 0, 0),
(220601, 2, 4995.53, 573.623, 2.59303, 100, 0, 0, 0),
(220601, 3, 4992.63, 580.705, 1.36818, 100, 0, 0, 0),
(220601, 4, 4987.8, 545.608, 5.24939, 100, 0, 0, 0),
(220601, 5, 4988.22, 547.721, 5.15979, 100, 0, 0, 0),
(220601, 6, 4985.59, 548.37, 5.20154, 100, 0, 0, 0),
(220601, 7, 4983.18, 548.91, 5.36621, 100, 0, 0, 0),
(220601, 8, 4987.79, 545.559, 5.25525, 100, 0, 0, 0),
(220601, 9, 4983.09, 549.002, 5.37683, 100, 0, 0, 0),
(220601, 10,4987.28, 544.82, 5.46887, 100, 0, 0, 0);

-- Waypoints for Murkdeep
INSERT INTO `creature_movement_special` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `waittime`, `wander_distance`, `script_id`) VALUES
(1032301, 1, 4991.55, 594.433, -0.65428, 100, 0, 0, 0),
(1032301, 2, 4987.94, 573.735, 2.45461, 100, 0, 0, 0),
(1032301, 3, 4981.71, 585.056, 0.535078, 100, 0, 0, 0);


-- End of migration.
END IF;
END??
delimiter ; 
CALL add_migration();
DROP PROCEDURE IF EXISTS add_migration;

