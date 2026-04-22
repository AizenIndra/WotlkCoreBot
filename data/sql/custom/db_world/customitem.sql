DELETE FROM `item_template` WHERE (entry = 973);
INSERT INTO `item_template` (
    `entry`, `class`, `subclass`, `SoundOverrideSubclass`, `name`, `displayid`,
    `Quality`, `Flags`, `FlagsExtra`, `BuyCount`, `BuyPrice`, `SellPrice`,
    `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`,
    `maxcount`, `stackable`,
    `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellcooldown_1`,
    `bonding`, `description`,
    `ScriptName`, `VerifiedBuild`
) VALUES (
    973,            -- entry
    9,              -- class (Recipe/Token in your setup)
    0,              -- subclass
    -1,             -- SoundOverrideSubclass
    'Unique Helper',
    1103,           -- displayid
    5,              -- quality
    0,              -- flags
    0,              -- flagsExtra
    1,              -- BuyCount
    0,              -- BuyPrice
    0,              -- SellPrice
    0,              -- InventoryType
    -1,             -- AllowableClass
    -1,             -- AllowableRace
    1,              -- ItemLevel
    0,              -- RequiredLevel
    2000,           -- maxcount
    100,            -- stackable
    14208,          -- spellid_1
    0,              -- spelltrigger_1 (on use)
    0,              -- spellcharges_1
    -1,             -- spellcooldown_1
    0,              -- bonding
    'Click me!',
    'custom_item',
    12340
);

DELETE FROM `item_template_locale` where `ID` IN (973);
INSERT INTO `item_template_locale` (`ID`, `locale`, `Name`, `Description`, `VerifiedBuild`) VALUES ('973', 'ruRU', 'Уникальный помощник', 'Нажми на меня!', '-12340');