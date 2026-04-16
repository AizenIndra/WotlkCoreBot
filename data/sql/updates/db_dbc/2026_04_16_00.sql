-- DB/DBC: optional one-time copy of client-data mirror tables from acore_world into acore_dbc
-- Runs only if legacy *_dbc tables still exist in acore_world (e.g. spell_dbc).
-- If *_dbc were already removed from acore_world, this is a no-op (data comes from base/db_dbc + updates).

DROP PROCEDURE IF EXISTS `acore_dbc_migrate_from_world_20260416`;
DELIMITER //
CREATE PROCEDURE `acore_dbc_migrate_from_world_20260416`()
BEGIN
  DECLARE legacy INT DEFAULT 0;
  SELECT COUNT(*) INTO legacy FROM information_schema.tables
    WHERE table_schema = 'acore_world' AND table_name = 'spell_dbc';
  IF legacy > 0 THEN
    SET NAMES utf8mb4;
    SET FOREIGN_KEY_CHECKS=0;
    SET UNIQUE_CHECKS=0;
    
    -- achievement_category_dbc
    TRUNCATE TABLE `achievement_category_dbc`;
    INSERT INTO `achievement_category_dbc` SELECT * FROM `acore_world`.`achievement_category_dbc`;
    
    -- achievement_criteria_dbc
    TRUNCATE TABLE `achievement_criteria_dbc`;
    INSERT INTO `achievement_criteria_dbc` SELECT * FROM `acore_world`.`achievement_criteria_dbc`;
    
    -- achievement_dbc
    TRUNCATE TABLE `achievement_dbc`;
    INSERT INTO `achievement_dbc` SELECT * FROM `acore_world`.`achievement_dbc`;
    
    -- areagroup_dbc
    TRUNCATE TABLE `areagroup_dbc`;
    INSERT INTO `areagroup_dbc` SELECT * FROM `acore_world`.`areagroup_dbc`;
    
    -- areapoi_dbc
    TRUNCATE TABLE `areapoi_dbc`;
    INSERT INTO `areapoi_dbc` SELECT * FROM `acore_world`.`areapoi_dbc`;
    
    -- areatable_dbc
    TRUNCATE TABLE `areatable_dbc`;
    INSERT INTO `areatable_dbc` SELECT * FROM `acore_world`.`areatable_dbc`;
    
    -- auctionhouse_dbc
    TRUNCATE TABLE `auctionhouse_dbc`;
    INSERT INTO `auctionhouse_dbc` SELECT * FROM `acore_world`.`auctionhouse_dbc`;
    
    -- bankbagslotprices_dbc
    TRUNCATE TABLE `bankbagslotprices_dbc`;
    INSERT INTO `bankbagslotprices_dbc` SELECT * FROM `acore_world`.`bankbagslotprices_dbc`;
    
    -- barbershopstyle_dbc
    TRUNCATE TABLE `barbershopstyle_dbc`;
    INSERT INTO `barbershopstyle_dbc` SELECT * FROM `acore_world`.`barbershopstyle_dbc`;
    
    -- battlemasterlist_dbc
    TRUNCATE TABLE `battlemasterlist_dbc`;
    INSERT INTO `battlemasterlist_dbc` SELECT * FROM `acore_world`.`battlemasterlist_dbc`;
    
    -- charstartoutfit_dbc
    TRUNCATE TABLE `charstartoutfit_dbc`;
    INSERT INTO `charstartoutfit_dbc` SELECT * FROM `acore_world`.`charstartoutfit_dbc`;
    
    -- chartitles_dbc
    TRUNCATE TABLE `chartitles_dbc`;
    INSERT INTO `chartitles_dbc` SELECT * FROM `acore_world`.`chartitles_dbc`;
    
    -- chatchannels_dbc
    TRUNCATE TABLE `chatchannels_dbc`;
    INSERT INTO `chatchannels_dbc` SELECT * FROM `acore_world`.`chatchannels_dbc`;
    
    -- chrclasses_dbc
    TRUNCATE TABLE `chrclasses_dbc`;
    INSERT INTO `chrclasses_dbc` SELECT * FROM `acore_world`.`chrclasses_dbc`;
    
    -- chrraces_dbc
    TRUNCATE TABLE `chrraces_dbc`;
    INSERT INTO `chrraces_dbc` SELECT * FROM `acore_world`.`chrraces_dbc`;
    
    -- cinematiccamera_dbc
    TRUNCATE TABLE `cinematiccamera_dbc`;
    INSERT INTO `cinematiccamera_dbc` SELECT * FROM `acore_world`.`cinematiccamera_dbc`;
    
    -- cinematicsequences_dbc
    TRUNCATE TABLE `cinematicsequences_dbc`;
    INSERT INTO `cinematicsequences_dbc` SELECT * FROM `acore_world`.`cinematicsequences_dbc`;
    
    -- creaturedisplayinfo_dbc
    TRUNCATE TABLE `creaturedisplayinfo_dbc`;
    INSERT INTO `creaturedisplayinfo_dbc` SELECT * FROM `acore_world`.`creaturedisplayinfo_dbc`;
    
    -- creaturedisplayinfoextra_dbc
    TRUNCATE TABLE `creaturedisplayinfoextra_dbc`;
    INSERT INTO `creaturedisplayinfoextra_dbc` SELECT * FROM `acore_world`.`creaturedisplayinfoextra_dbc`;
    
    -- creaturefamily_dbc
    TRUNCATE TABLE `creaturefamily_dbc`;
    INSERT INTO `creaturefamily_dbc` SELECT * FROM `acore_world`.`creaturefamily_dbc`;
    
    -- creaturemodeldata_dbc
    TRUNCATE TABLE `creaturemodeldata_dbc`;
    INSERT INTO `creaturemodeldata_dbc` SELECT * FROM `acore_world`.`creaturemodeldata_dbc`;
    
    -- creaturespelldata_dbc
    TRUNCATE TABLE `creaturespelldata_dbc`;
    INSERT INTO `creaturespelldata_dbc` SELECT * FROM `acore_world`.`creaturespelldata_dbc`;
    
    -- creaturetype_dbc
    TRUNCATE TABLE `creaturetype_dbc`;
    INSERT INTO `creaturetype_dbc` SELECT * FROM `acore_world`.`creaturetype_dbc`;
    
    -- currencytypes_dbc
    TRUNCATE TABLE `currencytypes_dbc`;
    INSERT INTO `currencytypes_dbc` SELECT * FROM `acore_world`.`currencytypes_dbc`;
    
    -- destructiblemodeldata_dbc
    TRUNCATE TABLE `destructiblemodeldata_dbc`;
    INSERT INTO `destructiblemodeldata_dbc` SELECT * FROM `acore_world`.`destructiblemodeldata_dbc`;
    
    -- dungeonencounter_dbc
    TRUNCATE TABLE `dungeonencounter_dbc`;
    INSERT INTO `dungeonencounter_dbc` SELECT * FROM `acore_world`.`dungeonencounter_dbc`;
    
    -- durabilitycosts_dbc
    TRUNCATE TABLE `durabilitycosts_dbc`;
    INSERT INTO `durabilitycosts_dbc` SELECT * FROM `acore_world`.`durabilitycosts_dbc`;
    
    -- durabilityquality_dbc
    TRUNCATE TABLE `durabilityquality_dbc`;
    INSERT INTO `durabilityquality_dbc` SELECT * FROM `acore_world`.`durabilityquality_dbc`;
    
    -- emotes_dbc
    TRUNCATE TABLE `emotes_dbc`;
    INSERT INTO `emotes_dbc` SELECT * FROM `acore_world`.`emotes_dbc`;
    
    -- emotestext_dbc
    TRUNCATE TABLE `emotestext_dbc`;
    INSERT INTO `emotestext_dbc` SELECT * FROM `acore_world`.`emotestext_dbc`;
    
    -- faction_dbc
    TRUNCATE TABLE `faction_dbc`;
    INSERT INTO `faction_dbc` SELECT * FROM `acore_world`.`faction_dbc`;
    
    -- factiontemplate_dbc
    TRUNCATE TABLE `factiontemplate_dbc`;
    INSERT INTO `factiontemplate_dbc` SELECT * FROM `acore_world`.`factiontemplate_dbc`;
    
    -- gameobjectartkit_dbc
    TRUNCATE TABLE `gameobjectartkit_dbc`;
    INSERT INTO `gameobjectartkit_dbc` SELECT * FROM `acore_world`.`gameobjectartkit_dbc`;
    
    -- gameobjectdisplayinfo_dbc
    TRUNCATE TABLE `gameobjectdisplayinfo_dbc`;
    INSERT INTO `gameobjectdisplayinfo_dbc` SELECT * FROM `acore_world`.`gameobjectdisplayinfo_dbc`;
    
    -- gemproperties_dbc
    TRUNCATE TABLE `gemproperties_dbc`;
    INSERT INTO `gemproperties_dbc` SELECT * FROM `acore_world`.`gemproperties_dbc`;
    
    -- glyphproperties_dbc
    TRUNCATE TABLE `glyphproperties_dbc`;
    INSERT INTO `glyphproperties_dbc` SELECT * FROM `acore_world`.`glyphproperties_dbc`;
    
    -- glyphslot_dbc
    TRUNCATE TABLE `glyphslot_dbc`;
    INSERT INTO `glyphslot_dbc` SELECT * FROM `acore_world`.`glyphslot_dbc`;
    
    -- gtbarbershopcostbase_dbc
    TRUNCATE TABLE `gtbarbershopcostbase_dbc`;
    INSERT INTO `gtbarbershopcostbase_dbc` SELECT * FROM `acore_world`.`gtbarbershopcostbase_dbc`;
    
    -- gtchancetomeleecrit_dbc
    TRUNCATE TABLE `gtchancetomeleecrit_dbc`;
    INSERT INTO `gtchancetomeleecrit_dbc` SELECT * FROM `acore_world`.`gtchancetomeleecrit_dbc`;
    
    -- gtchancetomeleecritbase_dbc
    TRUNCATE TABLE `gtchancetomeleecritbase_dbc`;
    INSERT INTO `gtchancetomeleecritbase_dbc` SELECT * FROM `acore_world`.`gtchancetomeleecritbase_dbc`;
    
    -- gtchancetospellcrit_dbc
    TRUNCATE TABLE `gtchancetospellcrit_dbc`;
    INSERT INTO `gtchancetospellcrit_dbc` SELECT * FROM `acore_world`.`gtchancetospellcrit_dbc`;
    
    -- gtchancetospellcritbase_dbc
    TRUNCATE TABLE `gtchancetospellcritbase_dbc`;
    INSERT INTO `gtchancetospellcritbase_dbc` SELECT * FROM `acore_world`.`gtchancetospellcritbase_dbc`;
    
    -- gtcombatratings_dbc
    TRUNCATE TABLE `gtcombatratings_dbc`;
    INSERT INTO `gtcombatratings_dbc` SELECT * FROM `acore_world`.`gtcombatratings_dbc`;
    
    -- gtnpcmanacostscaler_dbc
    TRUNCATE TABLE `gtnpcmanacostscaler_dbc`;
    INSERT INTO `gtnpcmanacostscaler_dbc` SELECT * FROM `acore_world`.`gtnpcmanacostscaler_dbc`;
    
    -- gtoctclasscombatratingscalar_dbc
    TRUNCATE TABLE `gtoctclasscombatratingscalar_dbc`;
    INSERT INTO `gtoctclasscombatratingscalar_dbc` SELECT * FROM `acore_world`.`gtoctclasscombatratingscalar_dbc`;
    
    -- gtoctregenhp_dbc
    TRUNCATE TABLE `gtoctregenhp_dbc`;
    INSERT INTO `gtoctregenhp_dbc` SELECT * FROM `acore_world`.`gtoctregenhp_dbc`;
    
    -- gtregenhpperspt_dbc
    TRUNCATE TABLE `gtregenhpperspt_dbc`;
    INSERT INTO `gtregenhpperspt_dbc` SELECT * FROM `acore_world`.`gtregenhpperspt_dbc`;
    
    -- gtregenmpperspt_dbc
    TRUNCATE TABLE `gtregenmpperspt_dbc`;
    INSERT INTO `gtregenmpperspt_dbc` SELECT * FROM `acore_world`.`gtregenmpperspt_dbc`;
    
    -- holidays_dbc
    TRUNCATE TABLE `holidays_dbc`;
    INSERT INTO `holidays_dbc` SELECT * FROM `acore_world`.`holidays_dbc`;
    
    -- item_dbc
    TRUNCATE TABLE `item_dbc`;
    INSERT INTO `item_dbc` SELECT * FROM `acore_world`.`item_dbc`;
    
    -- itembagfamily_dbc
    TRUNCATE TABLE `itembagfamily_dbc`;
    INSERT INTO `itembagfamily_dbc` SELECT * FROM `acore_world`.`itembagfamily_dbc`;
    
    -- itemdisplayinfo_dbc
    TRUNCATE TABLE `itemdisplayinfo_dbc`;
    INSERT INTO `itemdisplayinfo_dbc` SELECT * FROM `acore_world`.`itemdisplayinfo_dbc`;
    
    -- itemextendedcost_dbc
    TRUNCATE TABLE `itemextendedcost_dbc`;
    INSERT INTO `itemextendedcost_dbc` SELECT * FROM `acore_world`.`itemextendedcost_dbc`;
    
    -- itemlimitcategory_dbc
    TRUNCATE TABLE `itemlimitcategory_dbc`;
    INSERT INTO `itemlimitcategory_dbc` SELECT * FROM `acore_world`.`itemlimitcategory_dbc`;
    
    -- itemrandomproperties_dbc
    TRUNCATE TABLE `itemrandomproperties_dbc`;
    INSERT INTO `itemrandomproperties_dbc` SELECT * FROM `acore_world`.`itemrandomproperties_dbc`;
    
    -- itemrandomsuffix_dbc
    TRUNCATE TABLE `itemrandomsuffix_dbc`;
    INSERT INTO `itemrandomsuffix_dbc` SELECT * FROM `acore_world`.`itemrandomsuffix_dbc`;
    
    -- itemset_dbc
    TRUNCATE TABLE `itemset_dbc`;
    INSERT INTO `itemset_dbc` SELECT * FROM `acore_world`.`itemset_dbc`;
    
    -- lfgdungeons_dbc
    TRUNCATE TABLE `lfgdungeons_dbc`;
    INSERT INTO `lfgdungeons_dbc` SELECT * FROM `acore_world`.`lfgdungeons_dbc`;
    
    -- light_dbc
    TRUNCATE TABLE `light_dbc`;
    INSERT INTO `light_dbc` SELECT * FROM `acore_world`.`light_dbc`;
    
    -- liquidtype_dbc
    TRUNCATE TABLE `liquidtype_dbc`;
    INSERT INTO `liquidtype_dbc` SELECT * FROM `acore_world`.`liquidtype_dbc`;
    
    -- lock_dbc
    TRUNCATE TABLE `lock_dbc`;
    INSERT INTO `lock_dbc` SELECT * FROM `acore_world`.`lock_dbc`;
    
    -- mailtemplate_dbc
    TRUNCATE TABLE `mailtemplate_dbc`;
    INSERT INTO `mailtemplate_dbc` SELECT * FROM `acore_world`.`mailtemplate_dbc`;
    
    -- map_dbc
    TRUNCATE TABLE `map_dbc`;
    INSERT INTO `map_dbc` SELECT * FROM `acore_world`.`map_dbc`;
    
    -- mapdifficulty_dbc
    TRUNCATE TABLE `mapdifficulty_dbc`;
    INSERT INTO `mapdifficulty_dbc` SELECT * FROM `acore_world`.`mapdifficulty_dbc`;
    
    -- movie_dbc
    TRUNCATE TABLE `movie_dbc`;
    INSERT INTO `movie_dbc` SELECT * FROM `acore_world`.`movie_dbc`;
    
    -- namesprofanity_dbc
    TRUNCATE TABLE `namesprofanity_dbc`;
    INSERT INTO `namesprofanity_dbc` SELECT * FROM `acore_world`.`namesprofanity_dbc`;
    
    -- namesreserved_dbc
    TRUNCATE TABLE `namesreserved_dbc`;
    INSERT INTO `namesreserved_dbc` SELECT * FROM `acore_world`.`namesreserved_dbc`;
    
    -- overridespelldata_dbc
    TRUNCATE TABLE `overridespelldata_dbc`;
    INSERT INTO `overridespelldata_dbc` SELECT * FROM `acore_world`.`overridespelldata_dbc`;
    
    -- powerdisplay_dbc
    TRUNCATE TABLE `powerdisplay_dbc`;
    INSERT INTO `powerdisplay_dbc` SELECT * FROM `acore_world`.`powerdisplay_dbc`;
    
    -- pvpdifficulty_dbc
    TRUNCATE TABLE `pvpdifficulty_dbc`;
    INSERT INTO `pvpdifficulty_dbc` SELECT * FROM `acore_world`.`pvpdifficulty_dbc`;
    
    -- questfactionreward_dbc
    TRUNCATE TABLE `questfactionreward_dbc`;
    INSERT INTO `questfactionreward_dbc` SELECT * FROM `acore_world`.`questfactionreward_dbc`;
    
    -- questsort_dbc
    TRUNCATE TABLE `questsort_dbc`;
    INSERT INTO `questsort_dbc` SELECT * FROM `acore_world`.`questsort_dbc`;
    
    -- questxp_dbc
    TRUNCATE TABLE `questxp_dbc`;
    INSERT INTO `questxp_dbc` SELECT * FROM `acore_world`.`questxp_dbc`;
    
    -- randproppoints_dbc
    TRUNCATE TABLE `randproppoints_dbc`;
    INSERT INTO `randproppoints_dbc` SELECT * FROM `acore_world`.`randproppoints_dbc`;
    
    -- scalingstatdistribution_dbc
    TRUNCATE TABLE `scalingstatdistribution_dbc`;
    INSERT INTO `scalingstatdistribution_dbc` SELECT * FROM `acore_world`.`scalingstatdistribution_dbc`;
    
    -- scalingstatvalues_dbc
    TRUNCATE TABLE `scalingstatvalues_dbc`;
    INSERT INTO `scalingstatvalues_dbc` SELECT * FROM `acore_world`.`scalingstatvalues_dbc`;
    
    -- skillline_dbc
    TRUNCATE TABLE `skillline_dbc`;
    INSERT INTO `skillline_dbc` SELECT * FROM `acore_world`.`skillline_dbc`;
    
    -- skilllineability_dbc
    TRUNCATE TABLE `skilllineability_dbc`;
    INSERT INTO `skilllineability_dbc` SELECT * FROM `acore_world`.`skilllineability_dbc`;
    
    -- skillraceclassinfo_dbc
    TRUNCATE TABLE `skillraceclassinfo_dbc`;
    INSERT INTO `skillraceclassinfo_dbc` SELECT * FROM `acore_world`.`skillraceclassinfo_dbc`;
    
    -- skilltiers_dbc
    TRUNCATE TABLE `skilltiers_dbc`;
    INSERT INTO `skilltiers_dbc` SELECT * FROM `acore_world`.`skilltiers_dbc`;
    
    -- soundentries_dbc
    TRUNCATE TABLE `soundentries_dbc`;
    INSERT INTO `soundentries_dbc` SELECT * FROM `acore_world`.`soundentries_dbc`;
    
    -- spell_dbc
    TRUNCATE TABLE `spell_dbc`;
    INSERT INTO `spell_dbc` SELECT * FROM `acore_world`.`spell_dbc`;
    
    -- spellcasttimes_dbc
    TRUNCATE TABLE `spellcasttimes_dbc`;
    INSERT INTO `spellcasttimes_dbc` SELECT * FROM `acore_world`.`spellcasttimes_dbc`;
    
    -- spellcategory_dbc
    TRUNCATE TABLE `spellcategory_dbc`;
    INSERT INTO `spellcategory_dbc` SELECT * FROM `acore_world`.`spellcategory_dbc`;
    
    -- spelldifficulty_dbc
    TRUNCATE TABLE `spelldifficulty_dbc`;
    INSERT INTO `spelldifficulty_dbc` SELECT * FROM `acore_world`.`spelldifficulty_dbc`;
    
    -- spellduration_dbc
    TRUNCATE TABLE `spellduration_dbc`;
    INSERT INTO `spellduration_dbc` SELECT * FROM `acore_world`.`spellduration_dbc`;
    
    -- spellfocusobject_dbc
    TRUNCATE TABLE `spellfocusobject_dbc`;
    INSERT INTO `spellfocusobject_dbc` SELECT * FROM `acore_world`.`spellfocusobject_dbc`;
    
    -- spellitemenchantment_dbc
    TRUNCATE TABLE `spellitemenchantment_dbc`;
    INSERT INTO `spellitemenchantment_dbc` SELECT * FROM `acore_world`.`spellitemenchantment_dbc`;
    
    -- spellitemenchantmentcondition_dbc
    TRUNCATE TABLE `spellitemenchantmentcondition_dbc`;
    INSERT INTO `spellitemenchantmentcondition_dbc` SELECT * FROM `acore_world`.`spellitemenchantmentcondition_dbc`;
    
    -- spellradius_dbc
    TRUNCATE TABLE `spellradius_dbc`;
    INSERT INTO `spellradius_dbc` SELECT * FROM `acore_world`.`spellradius_dbc`;
    
    -- spellrange_dbc
    TRUNCATE TABLE `spellrange_dbc`;
    INSERT INTO `spellrange_dbc` SELECT * FROM `acore_world`.`spellrange_dbc`;
    
    -- spellrunecost_dbc
    TRUNCATE TABLE `spellrunecost_dbc`;
    INSERT INTO `spellrunecost_dbc` SELECT * FROM `acore_world`.`spellrunecost_dbc`;
    
    -- spellshapeshiftform_dbc
    TRUNCATE TABLE `spellshapeshiftform_dbc`;
    INSERT INTO `spellshapeshiftform_dbc` SELECT * FROM `acore_world`.`spellshapeshiftform_dbc`;
    
    -- spellvisual_dbc
    TRUNCATE TABLE `spellvisual_dbc`;
    INSERT INTO `spellvisual_dbc` SELECT * FROM `acore_world`.`spellvisual_dbc`;
    
    -- stableslotprices_dbc
    TRUNCATE TABLE `stableslotprices_dbc`;
    INSERT INTO `stableslotprices_dbc` SELECT * FROM `acore_world`.`stableslotprices_dbc`;
    
    -- summonproperties_dbc
    TRUNCATE TABLE `summonproperties_dbc`;
    INSERT INTO `summonproperties_dbc` SELECT * FROM `acore_world`.`summonproperties_dbc`;
    
    -- talent_dbc
    TRUNCATE TABLE `talent_dbc`;
    INSERT INTO `talent_dbc` SELECT * FROM `acore_world`.`talent_dbc`;
    
    -- talenttab_dbc
    TRUNCATE TABLE `talenttab_dbc`;
    INSERT INTO `talenttab_dbc` SELECT * FROM `acore_world`.`talenttab_dbc`;
    
    -- taxinodes_dbc
    TRUNCATE TABLE `taxinodes_dbc`;
    INSERT INTO `taxinodes_dbc` SELECT * FROM `acore_world`.`taxinodes_dbc`;
    
    -- taxipath_dbc
    TRUNCATE TABLE `taxipath_dbc`;
    INSERT INTO `taxipath_dbc` SELECT * FROM `acore_world`.`taxipath_dbc`;
    
    -- taxipathnode_dbc
    TRUNCATE TABLE `taxipathnode_dbc`;
    INSERT INTO `taxipathnode_dbc` SELECT * FROM `acore_world`.`taxipathnode_dbc`;
    
    -- teamcontributionpoints_dbc
    TRUNCATE TABLE `teamcontributionpoints_dbc`;
    INSERT INTO `teamcontributionpoints_dbc` SELECT * FROM `acore_world`.`teamcontributionpoints_dbc`;
    
    -- totemcategory_dbc
    TRUNCATE TABLE `totemcategory_dbc`;
    INSERT INTO `totemcategory_dbc` SELECT * FROM `acore_world`.`totemcategory_dbc`;
    
    -- transportanimation_dbc
    TRUNCATE TABLE `transportanimation_dbc`;
    INSERT INTO `transportanimation_dbc` SELECT * FROM `acore_world`.`transportanimation_dbc`;
    
    -- transportrotation_dbc
    TRUNCATE TABLE `transportrotation_dbc`;
    INSERT INTO `transportrotation_dbc` SELECT * FROM `acore_world`.`transportrotation_dbc`;
    
    -- vehicle_dbc
    TRUNCATE TABLE `vehicle_dbc`;
    INSERT INTO `vehicle_dbc` SELECT * FROM `acore_world`.`vehicle_dbc`;
    
    -- vehicleseat_dbc
    TRUNCATE TABLE `vehicleseat_dbc`;
    INSERT INTO `vehicleseat_dbc` SELECT * FROM `acore_world`.`vehicleseat_dbc`;
    
    -- wmoareatable_dbc
    TRUNCATE TABLE `wmoareatable_dbc`;
    INSERT INTO `wmoareatable_dbc` SELECT * FROM `acore_world`.`wmoareatable_dbc`;
    
    -- worldmaparea_dbc
    TRUNCATE TABLE `worldmaparea_dbc`;
    INSERT INTO `worldmaparea_dbc` SELECT * FROM `acore_world`.`worldmaparea_dbc`;
    
    -- worldmapoverlay_dbc
    TRUNCATE TABLE `worldmapoverlay_dbc`;
    INSERT INTO `worldmapoverlay_dbc` SELECT * FROM `acore_world`.`worldmapoverlay_dbc`;
    
    SET UNIQUE_CHECKS=1;
    SET FOREIGN_KEY_CHECKS=1;
  END IF;
END//
DELIMITER ;
CALL `acore_dbc_migrate_from_world_20260416`();
DROP PROCEDURE IF EXISTS `acore_dbc_migrate_from_world_20260416`;
