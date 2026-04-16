-- DB update mirror of custom: anticheat GM strings (LANG_GM_ANNOUNCE_* 30128–30139)
DELETE FROM `acore_string` WHERE `entry` BETWEEN 30128 AND 30139;

INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(30128, '|cffff0000[Anticheat]|r SpeedHack Detected for %s, normal distance for this time and speed = %f, distance from packet = %f', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(30129, '|cffff0000[Anticheat]|r FlyHack Detected for %s, player cannot fly', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(30130, '|cffff0000[Anticheat]|r FlyHack Detected for %s, player has Swimming flag, but not in water', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(30131, '|cffff0000[Anticheat]|r FlyHack Detected for %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(30132, '|cffff0000[Anticheat]|r DoubleJump Detected for %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(30133, '|cffff0000[Anticheat]|r Fake Jumper Detected for %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(30134, '|cffff0000[Anticheat]|r FakeFlying mode Detected for %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(30135, '|cffff0000[Anticheat]|r Wallclimb Detected for Account id: %u, Player %s, diffZ = %f, distance = %f, angle = %f, Map = %s, mapId = %u, X = %f, Y = %f, Z = %f', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(30136, '|cffff0000[Anticheat]|r Ignore control Hack Detected for Account: %u, Player: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(30137, '|cffff0000[Anticheat]|r NoFallingDamage Hack Detected for Account: %u, Player: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(30138, '|cffff0000[Anticheat]|r Possible water-walk hack: player %s (surface band without aura).', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(30139, '|cffff0000[Anticheat]|r MegaJump: player %s (zspeed=%f, max allowed ~%f).', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
