# Анализ системы Premium (VIP)

## Обзор

Система премиум-подписок даёт игрокам бонусы: повышенные рейты, VIP-команды (дебафф, банк, почта и т.д.), отсутствие потери прочности в определённых условиях. Премиум привязан к аккаунту и имеет срок действия.

---

## Архитектура

### 1. Хранилище данных

**Таблица `account_premium` (acore_auth):**
| Колонка   | Описание                                      |
|-----------|-----------------------------------------------|
| id        | Account ID                                    |
| setdate   | Unix timestamp установки премиума             |
| unsetdate | Unix timestamp окончания премиума             |
| active    | 1 = активен, 0 = неактивен                    |

**Примечание:** `LOGIN_GET_ACCOUNT_PREMIUM_STATUS_BY_ID` проверяет `active = 1` и `id`, но **не проверяет** `unsetdate` — истечение по времени обрабатывается на стороне приложения.

### 2. Компоненты

| Компонент               | Файл                           | Роль                                      |
|-------------------------|--------------------------------|-------------------------------------------|
| AccountMgr              | `Accounts/AccountMgr.cpp`      | CRUD премиума в БД                        |
| Player                  | `Entities/Player/*`            | `m_vip`, `m_unsetdate`, `m_premiumTimer`  |
| WorldSession            | `Handlers/CharacterHandler.cpp`| `LoadPremiumStatusToPlayer` при логине    |
| Player::Update          | `Entities/Player/PlayerUpdates.cpp` | Проверка истечения, снятие дебаффа  |
| AddonIO                 | `Addons/AddonIO.cpp`           | Покупка премиума, информация аддону       |
| cs_vip                  | `scripts/Commands/cs_vip.cpp`  | VIP-команды (.vip debuff, bank и т.д.)    |

### 3. Поток данных

```
Логин → LoadPremiumStatusToPlayer() 
  → GetVipStatus(accountId), GetVIPunsetDate(accountId)
  → если unsetdate < now: RemoveVipStatus, снять дебафф
  → SetPremiumStatus(vip), SetPremiumUnsetdate(unsetdate)

Покупка (AddonIO) → HandlePremiumRenewRequest
  → SetAccountCurrency (списание бонусов)
  → SetVipStatus / UpdateVipStatus
  → SetPremiumStatus(true), SetPremiumUnsetdate(...)

Игровой цикл → Player::Update
  → каждые ~60 сек: проверка unset <= now
  → при истечении: RemoveVipStatus, RemoveAurasDueToSpell, SetPremiumStatus(false)
  → при !m_vip: снятие VIP-дебаффа (защита от рассинхрона)
```

---

## Конфигурация (worldserver.conf)

### Shop
- `Shop.Enabled` — включение магазина
- `ShopUpdateInterval` — интервал обновления магазина (мс)

### VIP-команды (вкл/выкл)
- `Config.Vip.Debuff.Command` / `Config.Vip.Debuff.Spell` (80867)
- `Config.Vip.Buff`, `Config.Vip.Buff.Spells`
- `Config.Vip.Bank`, `Config.Vip.Mail`, `Config.Vip.Repair`
- `Config.Vip.Reset.Talents`, `Config.Vip.Taxi`, `Config.Vip.Home`
- `Config.Vip.Capital`, `Config.Vip.Change.Race`, `Config.Vip.Customize`, `Config.Vip.Appear`

### Рейты для премиума
- `Rate.XP.Kill.Premium`, `Rate.XP.Quest.Premium`, `Rate.XP.Quest.DF.Premium`
- `Rate.XP.Explore.Premium`, `Rate.Honor.Premium`, `Rate.Reputation.Gain.Premium`
- `SkillGain.Crafting.Premium`, `SkillGain.Gathering.Premium`

---

## VIP-команды

| Команда         | Суть                              | Статус реализации        |
|-----------------|-----------------------------------|---------------------------|
| .vip debuff     | Снять негативные ауры + наложить дебафф | Работает             |
| .vip buff       | Наложить баффы из конфига         | Работает                  |
| .vip bank       | Открыть банк                      | Работает                  |
| .vip mail       | Открыть почту                     | Работает                  |
| .vip repair     | Ремонт                            | **Заглушка** (LANG_VIP_ERROR) |
| .vip taxi       | Такси                             | **Заглушка**              |
| .vip home       | Телепорт домой                    | Работает                  |
| .vip capital    | Телепорт в столицу                | Работает                  |
| .vip resettalents | Сброс талантов                  | Работает                  |
| .vip changerace | Смена расы                        | **Заглушка**              |
| .vip customize  | Кастомизация                      | **Заглушка**              |
| .vip appear     | Появление                         | **Заглушка**              |

---

## Где учитывается IsPremium()

- **Player.cpp:** прочность (нет потерь при смерти), XP за исследование, честь, XP за квесты
- **Formulas.cpp:** XP за убийство
- **ReputationMgr.cpp:** репутация
- **PlayerUpdates.cpp:** бонусы за скиллы (крафт/сбор)
- **MiscHandler.cpp:** отмена AFK при ресте

---

## Покупка премиума (AddonIO)

**Тарифы (хардкод в AddonIO.cpp):**
| Вариант | Стоимость (бонусы) | Время           |
|---------|--------------------|-----------------|
| 1       | 1                  | 1 день (86400)  |
| 2       | 6                  | 7 дней          |
| 3       | 12                 | 14 дней         |
| 4       | 25                 | 30 дней         |

Валюта: `account_donate.bonuses` (moneyID=1). Покупка через аддон: `ACMSG_PREMIUM_RENEW_REQUEST` с `mes` = 1–4.

---

## Возможные проблемы

### 1. **LOGIN_SET_ACCOUNT_PREMIUM — INSERT при уже существующей записи**

`SetVipStatus` использует `INSERT INTO account_premium`. Если по какой-то причине запись для аккаунта уже есть (например, ручное обновление БД), INSERT завершится ошибкой дубликата ключа.

**Рекомендация:** перейти на `INSERT ... ON DUPLICATE KEY UPDATE` или `REPLACE INTO` в соответствующем prepared statement.

### 2. **GetVipStatus не учитывает unsetdate**

`LOGIN_GET_ACCOUNT_PREMIUM_STATUS_BY_ID` проверяет только `active = 1`, а не `unsetdate > NOW()`. Поэтому логика истечения по времени полностью на стороне `LoadPremiumStatusToPlayer` и `Player::Update`. Это допустимо, но при прямых запросах к `GetVipStatus` без проверки `unsetdate` возможны несоответствия.

### 3. **Единицы измерения m_premiumTimer**

`m_premiumTimer = 1000 * MINUTE` — результат 60000. В `Player::Update` `p_time` в миллисекундах, значит проверка раз в ~60 секунд. `MINUTE` определён в секундах (60), `1000 * 60` = 60000 — интерпретируется как 60 сек в мс. Следует добавить явный `* IN_MILLISECONDS` или константу для ясности.

### 4. **Неисправленные VIP-команды**

`.vip repair`, `.vip taxi`, `.vip changerace`, `.vip customize`, `.vip appear` возвращают LANG_VIP_ERROR и не реализованы. Либо реализовать, либо показывать отдельное сообщение «в разработке».

### 5. **AddonIO HandlePremiumRenewRequest — sess не инициализирован**

В `HandleShopBuyItemRequest` встречается использование `sess` до объявления. В `HandlePremiumRenewRequest` `sess` объявлен как `player->GetSession()`. Нужно убедиться, что во всех похожих обработчиках `sess` всегда инициализирован перед использованием.

### 6. **Проверка премиума при переключении персонажей**

`LoadPremiumStatusToPlayer` вызывается при `HandlePlayerLoginFromDB` и `HandlePlayerLoginToCharInWorld`. При смене персонажа статус подгружается заново — логика корректна.

### 7. **Опечатки в enum**

В AddonIO: `PAID_SERVICE_ALCHEMY_LEARH`, `PAID_SERVICE_FISHING_LEARH`, `PAID_SERVICE_ENGINEERIN_LEARN` — опечатки `LEARH` / `ENGINEERIN`. Стоит выровнять с остальными enum.

### 8. **HandlePremiumInfoRequest — возможное переполнение**

`player->GetPremiumUnsetdate() - uint32(time(nullptr))` при `unsetdate < time(nullptr)` даст большое значение из-за unsigned-переполнения. Клиент может не ожидать этого. Безопаснее передавать 0 при истёкшем премиуме.

---

## Идеи улучшений

1. **Кэш VIP в памяти**  
   Снизить количество запросов к `account_premium` (например, при каждом вызове `GetVipStatus`) кэшированием с периодическим обновлением.

2. **События при истечении премиума**  
   `ScriptMgr::OnPremiumExpired(player)` для скриптов (логирование, уведомления, отключение фич).

3. **Вынести тарифы в конфиг**  
   Стоимость и длительность премиума задавать в worldserver.conf вместо хардкода.

4. **Графический интерфейс срока премиума**  
   Команда `.vip time` для отображения оставшегося времени (через `secsToTimeString`).

5. **Проверка VIP при входе в инстанс/БГ**  
   Ограничить некоторые VIP-возможности в PvP/инстансах или явно описать это в документации.

6. **Логирование покупок**  
   `WritePurchaseToLogs` уже вызывается; стоит убедиться, что логи пишутся в отдельный файл/таблицу для аналитики.

---

## Схема взаимодействия

```
[Клиент/Аддон]
     │
     ├── ACMSG_PREMIUM_INFO_REQUEST  → ASMSG_PREMIUM_INFO_RESPONSE\t{seconds}
     └── ACMSG_PREMIUM_RENEW_REQUEST → ASMSG_PREMIUM_RENEW_RESPONSE\t{0|1}
              │
              ▼
     [AddonIO] HandlePremiumRenewRequest
              │
              ├── SetAccountCurrency (account_donate.bonuses)
              ├── AccountMgr::SetVipStatus / UpdateVipStatus
              └── player->SetPremiumStatus, SetPremiumUnsetdate

[Логин]
     │
     ▼
LoadPremiumStatusToPlayer
     ├── GetVipStatus, GetVIPunsetDate
     ├── при истечении: RemoveVipStatus, RemoveAurasDueToSpell
     └── SetPremiumStatus, SetPremiumUnsetdate

[Игровой цикл]
     │
     ▼
Player::Update (каждые ~60 сек)
     ├── при !m_vip: RemoveAurasDueToSpell (VIP debuff)
     └── при m_vip && unset <= now: RemoveVipStatus, SetPremiumStatus(false), снять дебафф
```
