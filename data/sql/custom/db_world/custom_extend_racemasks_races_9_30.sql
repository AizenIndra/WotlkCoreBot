-- Extend race bitmasks in DB for custom races (IDs 9–30) when a row already
-- allowed every classic WotLK race (mask 0x7FF: races 1–8, 10–11; slot 9 unused).
-- Races 9–30 use bits 8–29. OR-ing 0x3FFFFF00 into such rows yields full mask
-- 0x3FFFFFFF (races 1–30), matching SharedDefines CLASSMASK_ALL_PLAYABLE.
--
-- Do NOT run blindly if you rely on masks that intentionally omit some classic
-- races but happen to include 0x7FF bits — review first.
--
-- Transforms (Orb of Deception, Corsair costume, etc.) are handled in core
-- (SpellAuraEffects.cpp), not in SQL.

-- Use decimal literals: MySQL 8+ can treat 0x... in SET as binary string and break & / |
SET @MASK_CLASSIC_ALL := 2047;              /* 0x7FF: all 10 classic playable */
SET @MASK_EXTRA_9_30 := 1073741568;         /* 0x3FFFFF00: races 9–30; OR 2047 => 0x3FFFFFFF */

-- item_template.AllowableRace: -1 means all races (no change)
UPDATE `item_template`
SET `AllowableRace` = `AllowableRace` | @MASK_EXTRA_9_30
WHERE `AllowableRace` <> -1
  AND `AllowableRace` <> 0
  AND (`AllowableRace` & @MASK_CLASSIC_ALL) = @MASK_CLASSIC_ALL;

-- quest_template: AllowableRaces = 0 means any race (skip)
UPDATE `quest_template`
SET `AllowableRaces` = `AllowableRaces` | @MASK_EXTRA_9_30
WHERE `AllowableRaces` <> 0
  AND (`AllowableRaces` & @MASK_CLASSIC_ALL) = @MASK_CLASSIC_ALL;

-- spell_area: racemask 0 = any race (skip)
UPDATE `spell_area`
SET `racemask` = `racemask` | @MASK_EXTRA_9_30
WHERE `racemask` <> 0
  AND (`racemask` & @MASK_CLASSIC_ALL) = @MASK_CLASSIC_ALL;

-- Level-up mail rewards (raceMask must intersect player race)
UPDATE `mail_level_reward`
SET `raceMask` = `raceMask` | @MASK_EXTRA_9_30
WHERE `raceMask` <> 0
  AND (`raceMask` & @MASK_CLASSIC_ALL) = @MASK_CLASSIC_ALL;

-- Player create: optional rows with explicit full classic mask
UPDATE `playercreateinfo_skills`
SET `raceMask` = `raceMask` | @MASK_EXTRA_9_30
WHERE `raceMask` <> 0
  AND (`raceMask` & @MASK_CLASSIC_ALL) = @MASK_CLASSIC_ALL;

UPDATE `playercreateinfo_spell_custom`
SET `racemask` = `racemask` | @MASK_EXTRA_9_30
WHERE `racemask` <> 0
  AND (`racemask` & @MASK_CLASSIC_ALL) = @MASK_CLASSIC_ALL;

UPDATE `playercreateinfo_cast_spell`
SET `racemask` = `racemask` | @MASK_EXTRA_9_30
WHERE `racemask` <> 0
  AND (`racemask` & @MASK_CLASSIC_ALL) = @MASK_CLASSIC_ALL;
