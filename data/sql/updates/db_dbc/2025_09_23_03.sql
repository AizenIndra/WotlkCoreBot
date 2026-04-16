-- Split from db_world/2025_09_23_03.sql: DBC-only statements (acore_dbc)

-- DB update 2025_09_23_02 -> 2025_09_23_03
--
UPDATE `spell_dbc` SET
    `Attributes`=384,
    `CastingTimeIndex`=1,
    `DurationIndex`=3,
    `RangeIndex`=7,
    `EquippedItemClass`=-1,
    `SchoolMask`=1,
    `Effect_1`=76,
    `ImplicitTargetA_1`=18,
    `EffectMiscValue_1`=185541
WHERE `Id`=39797;
