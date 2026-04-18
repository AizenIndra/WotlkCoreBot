-- Guild Level System - acore_string entries

DELETE FROM `acore_string` WHERE `entry` IN (12750, 12751);

INSERT INTO `acore_string` (`entry`, `content_default`) VALUES
  -- Broadcast when guild levels up: {0} = guild name, {1} = new level
  (12750, 'Guild "{0}" has reached level {1}!'),
  -- Generic guild level system info/reset text (used by commands)
  (12751, 'Guild level system data has been reset.');

