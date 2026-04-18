-- Тестовый сид BattlePass: Сезон 1
-- Требует таблицы из:
--   2026_04_08_00_battlepass_world.sql

-- Удаляем предыдущий сид Сезона 1 (идемпотентность).
-- И отключаем все другие активные сезоны этих категорий, чтобы сервер не взял старый LIMIT 1.
UPDATE battlepass_season
SET active = 0
WHERE category IN ('PVE', 'PVP');

DELETE br
FROM battlepass_reward br
JOIN battlepass_season bs ON bs.id = br.season_id
WHERE bs.name = 'Сезон 1';

DELETE bt
FROM battlepass_task bt
JOIN battlepass_season bs ON bs.id = bt.season_id
WHERE bs.name = 'Сезон 1';

DELETE bb
FROM battlepass_bundle bb
JOIN battlepass_season bs ON bs.id = bb.season_id
WHERE bs.name = 'Сезон 1';

DELETE FROM battlepass_season
WHERE name = 'Сезон 1';

-- Один сезон (Сезон 1) в двух категориях: PVE и PVP.
-- Длительность 1 месяц (30 дней), 60 уровней БП.
INSERT INTO battlepass_season (category, name, active, start_time, end_time, premium_cost)
VALUES
('PVE', 'Сезон 1', 1, UNIX_TIMESTAMP(), UNIX_TIMESTAMP() + 2592000, 50),
('PVP', 'Сезон 1', 1, UNIX_TIMESTAMP(), UNIX_TIMESTAMP() + 2592000, 50);

-- 60 уникальных наград для Free и 60 уникальных наград для Premium.
-- ВНИМАНИЕ: используются кастомные item entry диапазоны без повторов.
INSERT INTO battlepass_reward
    (season_id, tier, free_item, free_count, premium_item, premium_count, xp_req, reward_type)
SELECT bs.id, r.tier, r.free_item, r.free_count, r.premium_item, r.premium_count, r.xp_req, 1
FROM battlepass_season bs
JOIN (
    SELECT 1 AS tier, 900001 AS free_item, 1 AS free_count, 910001 AS premium_item, 1 AS premium_count, 1000 AS xp_req
    UNION ALL SELECT 2, 900002, 2, 910002, 1, 2000
    UNION ALL SELECT 3, 900003, 2, 910003, 1, 3000
    UNION ALL SELECT 4, 900004, 3, 910004, 1, 4000
    UNION ALL SELECT 5, 900005, 1, 910005, 1, 5000
    UNION ALL SELECT 6, 900006, 2, 910006, 1, 6000
    UNION ALL SELECT 7, 900007, 2, 910007, 1, 7000
    UNION ALL SELECT 8, 900008, 3, 910008, 1, 8000
    UNION ALL SELECT 9, 900009, 1, 910009, 1, 9000
    UNION ALL SELECT 10, 900010, 1, 910010, 1, 10000
    UNION ALL SELECT 11, 900011, 2, 910011, 1, 11000
    UNION ALL SELECT 12, 900012, 2, 910012, 1, 12000
    UNION ALL SELECT 13, 900013, 3, 910013, 1, 13000
    UNION ALL SELECT 14, 900014, 1, 910014, 1, 14000
    UNION ALL SELECT 15, 900015, 2, 910015, 1, 15000
    UNION ALL SELECT 16, 900016, 2, 910016, 1, 16000
    UNION ALL SELECT 17, 900017, 3, 910017, 1, 17000
    UNION ALL SELECT 18, 900018, 1, 910018, 1, 18000
    UNION ALL SELECT 19, 900019, 2, 910019, 1, 19000
    UNION ALL SELECT 20, 900020, 1, 910020, 1, 20000
    UNION ALL SELECT 21, 900021, 2, 910021, 1, 21000
    UNION ALL SELECT 22, 900022, 2, 910022, 1, 22000
    UNION ALL SELECT 23, 900023, 3, 910023, 1, 23000
    UNION ALL SELECT 24, 900024, 1, 910024, 1, 24000
    UNION ALL SELECT 25, 900025, 2, 910025, 1, 25000
    UNION ALL SELECT 26, 900026, 2, 910026, 1, 26000
    UNION ALL SELECT 27, 900027, 3, 910027, 1, 27000
    UNION ALL SELECT 28, 900028, 1, 910028, 1, 28000
    UNION ALL SELECT 29, 900029, 2, 910029, 1, 29000
    UNION ALL SELECT 30, 900030, 1, 910030, 1, 30000
    UNION ALL SELECT 31, 900031, 2, 910031, 1, 31000
    UNION ALL SELECT 32, 900032, 2, 910032, 1, 32000
    UNION ALL SELECT 33, 900033, 3, 910033, 1, 33000
    UNION ALL SELECT 34, 900034, 1, 910034, 1, 34000
    UNION ALL SELECT 35, 900035, 2, 910035, 1, 35000
    UNION ALL SELECT 36, 900036, 2, 910036, 1, 36000
    UNION ALL SELECT 37, 900037, 3, 910037, 1, 37000
    UNION ALL SELECT 38, 900038, 1, 910038, 1, 38000
    UNION ALL SELECT 39, 900039, 2, 910039, 1, 39000
    UNION ALL SELECT 40, 900040, 1, 910040, 1, 40000
    UNION ALL SELECT 41, 900041, 2, 910041, 1, 41000
    UNION ALL SELECT 42, 900042, 2, 910042, 1, 42000
    UNION ALL SELECT 43, 900043, 3, 910043, 1, 43000
    UNION ALL SELECT 44, 900044, 1, 910044, 1, 44000
    UNION ALL SELECT 45, 900045, 2, 910045, 1, 45000
    UNION ALL SELECT 46, 900046, 2, 910046, 1, 46000
    UNION ALL SELECT 47, 900047, 3, 910047, 1, 47000
    UNION ALL SELECT 48, 900048, 1, 910048, 1, 48000
    UNION ALL SELECT 49, 900049, 2, 910049, 1, 49000
    UNION ALL SELECT 50, 900050, 1, 910050, 1, 50000
    UNION ALL SELECT 51, 900051, 2, 910051, 1, 51000
    UNION ALL SELECT 52, 900052, 2, 910052, 1, 52000
    UNION ALL SELECT 53, 900053, 3, 910053, 1, 53000
    UNION ALL SELECT 54, 900054, 1, 910054, 1, 54000
    UNION ALL SELECT 55, 900055, 2, 910055, 1, 55000
    UNION ALL SELECT 56, 900056, 2, 910056, 1, 56000
    UNION ALL SELECT 57, 900057, 3, 910057, 1, 57000
    UNION ALL SELECT 58, 900058, 1, 910058, 1, 58000
    UNION ALL SELECT 59, 900059, 2, 910059, 1, 59000
    UNION ALL SELECT 60, 900060, 1, 910060, 1, 60000
) r
WHERE bs.name = 'Сезон 1';

-- Новые задания для Сезона 1 (PVE).
INSERT INTO battlepass_task
    (season_id, category, task_type, event_type, target_id, title, description, requirement, reward_xp)
SELECT bs.id, 'PVE', t.task_type, t.event_type, t.target_id, t.title, t.description, t.requirement, t.reward_xp
FROM battlepass_season bs
JOIN (
    SELECT 'Daily' AS task_type, 'CREATURE_KILL' AS event_type, 0 AS target_id,
           'Охота на нечисть' AS title, 'Убейте 40 существ в открытом мире.' AS description,
           40 AS requirement, 450 AS reward_xp
    UNION ALL
    SELECT 'Daily', 'QUEST_COMPLETE', 0,
           'Поручения дня', 'Завершите 5 любых заданий.', 5, 500
    UNION ALL
    SELECT 'Daily', 'QUEST_COMPLETE', 0,
           'Опытный авантюрист', 'Завершите 8 любых заданий.', 8, 600
    UNION ALL
    SELECT 'Weekly', 'BG_WIN', 0,
           'Командный игрок', 'Победите на 5 полях боя.', 5, 1300
    UNION ALL
    SELECT 'Weekly', 'ARENA_WIN', 0,
           'Покоритель арены', 'Выиграйте 10 матчей арены.', 10, 1600
    UNION ALL
    SELECT 'Weekly', 'CREATURE_KILL', 31146,
           'Ликвидация цели', 'Убейте цель с entry 31146 двадцать раз.', 20, 1100
) t
WHERE bs.name = 'Сезон 1';

-- Новые задания для Сезона 1 (PVP).
INSERT INTO battlepass_task
    (season_id, category, task_type, event_type, target_id, title, description, requirement, reward_xp)
SELECT bs.id, 'PVP', t.task_type, t.event_type, t.target_id, t.title, t.description, t.requirement, t.reward_xp
FROM battlepass_season bs
JOIN (
    SELECT 'Daily' AS task_type, 'PVP_KILL' AS event_type, 0 AS target_id,
           'Дуэльная ярость' AS title, 'Совершите 10 PvP убийств.' AS description, 10 AS requirement, 700 AS reward_xp
    UNION ALL
    SELECT 'Daily', 'BG_WIN', 0,
           'Флаг у нас', 'Выиграйте 2 поля боя.', 2, 800
    UNION ALL
    SELECT 'Weekly', 'ARENA_WIN', 0,
           'Серия побед', 'Выиграйте 12 матчей арены за неделю.', 12, 1800
    UNION ALL
    SELECT 'Weekly', 'PVP_KILL', 0,
           'Без пощады', 'Совершите 60 PvP убийств за неделю.', 60, 2000
) t
WHERE bs.name = 'Сезон 1' AND bs.category = 'PVP';

-- Наборы опыта для Сезона 1.
INSERT INTO battlepass_bundle (season_id, category, name, cost, xp)
SELECT bs.id, bs.category, b.name, b.cost, b.xp
FROM battlepass_season bs
JOIN (
    SELECT 'Малый набор опыта' AS name, 10 AS cost, 1000 AS xp
    UNION ALL SELECT 'Средний набор опыта', 25, 3000
    UNION ALL SELECT 'Большой набор опыта', 40, 6000
) b
WHERE bs.name = 'Сезон 1';
