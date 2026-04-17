-- Premium/VIP acore_string entries
DELETE FROM `acore_string` WHERE `entry` IN (12001, 12170, 12171, 12172, 12173, 12174, 12175, 12176, 12177, 12178, 12179, 12180, 12181, 12182, 12183, 12184);
INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(12001, 'VIP mode', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Премиум режим'),
(12170, 'You are not a premium account.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'У вас нет премиум-аккаунта.'),
(12171, 'This premium command is disabled.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Эта премиум-команда отключена.'),
(12172, 'Premium error.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ошибка премиума.'),
(12173, 'You cannot do that in a battleground.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вы не можете сделать это на поле боя.'),
(12174, 'You cannot do that while in stealth.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вы не можете сделать это в режиме незаметности.'),
(12175, 'You cannot do that while dead.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вы не можете сделать это будучи мёртвым.'),
(12176, 'You cannot change race.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вы не можете сменить расу.'),
(12177, 'You cannot customize your character.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вы не можете изменить внешность персонажа.'),
(12178, 'You cannot do that in a group.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Вы не можете сделать это в группе.'),
(12179, 'Target is not a premium account.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'У цели нет премиум-аккаунта.'),
(12180, 'Premium time remaining: {}', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Осталось премиум-времени: {}'),
(12181, 'Your premium time will expire in less than 5 minutes.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ваше премиум-время истечёт менее чем через 5 минут.'),
(12182, 'Your premium time has expired.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ваше премиум-время истекло.'),
(12183, 'Target premium time remaining: {}', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Осталось премиум-времени у цели: {}'),
(12184, 'This feature is in development.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Эта функция в разработке.');