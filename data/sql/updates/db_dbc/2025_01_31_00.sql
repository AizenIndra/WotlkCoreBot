-- Split from db_world/2025_01_31_00.sql: DBC-only statements (acore_dbc)

-- Do not remove transform auras on evade
-- SPELL_ATTR1_AURA_STAYS_AFTER_COMBAT = 0x02000000, // TITLE Aura stays after combat DESCRIPTION Aura will not be removed when the unit leaves combat
UPDATE `spell_dbc` SET `AttributesEx`=`AttributesEx`|(0x02000000) WHERE `ID` IN (44918, 44919, 44920, 44921, 44922, 44923, 44924, 44925, 44926, 44927, 44928, 44929, 44930, 44931, 44932, 44962, 45155, 45156, 45157, 45158, 45159, 45160, 45161, 45162, 45163, 45164, 45165, 45166, 45167, 45168, 45169, 45170);
