-- Split from db_world/2025_04_02_02.sql: DBC-only statements (acore_dbc)

SET @SPELL_ATTR3_IGNORE_CASTER_AND_TARGET_RESTRICTIONS := 268435456;
SET @SPELL_ATTR5_ALLOW_WHILE_STUNNED := 8;
SET @SPELL_ATTR2_IGNORE_LINE_OF_SIGHT := 4;

-- Update server-side spell 43360 - model after 41295 Fixate
UPDATE `spell_dbc` SET
`AttributesEx2`= `AttributesEx2` & ~(@SPELL_ATTR2_IGNORE_LINE_OF_SIGHT),
`AttributesEx3`= `AttributesEx3` | @SPELL_ATTR3_IGNORE_CASTER_AND_TARGET_RESTRICTIONS,
`AttributesEx5`= `AttributesEx5` | @SPELL_ATTR5_ALLOW_WHILE_STUNNED,
`RangeIndex` = 36,
`Effect_1` = 6,
`EffectRadiusIndex_1` = 27,
`EffectAura_1` = 11,
`SpellIconID` = 1,
`ImplicitTargetA_1` = 7,
`Description_Lang_enUS` = 'The target is fixated upon the caster.',
`Name_Lang_enUS`='Fixate'
WHERE `ID` = 43360;
