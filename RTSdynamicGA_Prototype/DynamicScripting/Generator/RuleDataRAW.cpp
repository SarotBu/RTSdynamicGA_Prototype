//#include SOMETHING


//BUILD RULE
/*
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Self, IsTownHall());
    Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
    Units factorys = observation->GetUnits(Unit::Self, IsUnits(factory_types));
    Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
    Units barracks_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));
    Units factorys_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_FACTORYTECHLAB));
    Units starports_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_STARPORTTECHLAB));
    Units supply_depots = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_SUPPLYDEPOT));
*/
const ObservationInterface* observation = Observation();

void buildORBITALCOMMAND(){
    Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
    if (!barracks.empty()) {
        for (const auto& base : bases) {
            if (base->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER && observation->GetMinerals() > 150) {
                Actions()->UnitCommand(base, ABILITY_ID::MORPH_ORBITALCOMMAND);
            }
        }
    }
}

void buildBARRACKADDON(){
    Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
    for (const auto& barrack : barracks) {
        if (!barrack->orders.empty() || barrack->build_progress != 1) {
            continue;
        }
        if (observation->GetUnit(barrack->add_on_tag) == nullptr) {
            if (barracks_tech.size() < barracks.size() / 2 || barracks_tech.empty()) {
                TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_BARRACKS, barrack->tag);
            }
            else {
                TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_BARRACKS, barrack->tag);
            }
        }
    }
}

void buildFACTORY(){
    Units factorys = observation->GetUnits(Unit::Self, IsUnits(factory_types));
    for (const auto& factory : factorys) {
        if (!factory->orders.empty()) {
            continue;
        }

        if (observation->GetUnit(factory->add_on_tag) == nullptr) {
            if (mech_build_) {
                if (factorys_tech.size() < factorys.size() / 2) {
                    TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_FACTORY, factory->tag);
                }
                else {
                    TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_FACTORY, factory->tag);
                }
            }
            else {
                if (CountUnitType(observation, UNIT_TYPEID::TERRAN_BARRACKSREACTOR) < 1) {
                    TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_FACTORY, factory->tag);
                }
                else {
                    TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_FACTORY, factory->tag);
                }
            }

        }
    }
}

void buildSTARPORT(){
    Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
    for (const auto& starport : starports) {
        if (!starport->orders.empty()) {
            continue;
        }
        if (observation->GetUnit(starport->add_on_tag) == nullptr) {
            if (mech_build_) {
                if (CountUnitType(observation, UNIT_TYPEID::TERRAN_STARPORTREACTOR) < 2) {
                    TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_STARPORT, starport->tag);
                }
                else {
                    TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_STARPORT, starport->tag);
                }
            }
            else {
                if (CountUnitType(observation, UNIT_TYPEID::TERRAN_STARPORTREACTOR) < 1) {
                    TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_STARPORT, starport->tag);
                }
                else {
                    TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_STARPORT, starport->tag);
                }

            }
        }
    }
}

//RESEARCH RULE
/*
if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                    if (upgrade == UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1 && base_count > 2) {
                        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
                    }
                    else if (upgrade == UPGRADE_ID::TERRANINFANTRYARMORSLEVEL1 && base_count > 2) {
                        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
                    }
                    if (upgrade == UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2 && base_count > 4) {
                        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
                    }
                    else if (upgrade == UPGRADE_ID::TERRANINFANTRYARMORSLEVEL2 && base_count > 4) {
                        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
                    }
                }
                TryBuildUnit(ABILITY_ID::RESEARCH_STIMPACK, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
                TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
                TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
                TryBuildUnit(ABILITY_ID::RESEARCH_STIMPACK, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
                TryBuildUnit(ABILITY_ID::RESEARCH_COMBATSHIELD, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
                TryBuildUnit(ABILITY_ID::RESEARCH_CONCUSSIVESHELLS, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
                TryBuildUnit(ABILITY_ID::RESEARCH_PERSONALCLOAKING, UNIT_TYPEID::TERRAN_GHOSTACADEMY);
TryBuildUnit(ABILITY_ID::RESEARCH_BANSHEECLOAKINGFIELD, UNIT_TYPEID::TERRAN_STARPORTTECHLAB);
*/
void researchTERRANINFANTRYWEAPONSLEVEL1(){
    auto upgrades = observation->GetUpgrades();
    //for (const auto& upgrade : upgrades) {
        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
            TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
        }
    //}
}
void researchTERRANINFANTRYWEAPONSLEVEL2(){
    auto upgrades = observation->GetUpgrades();
    //for (const auto& upgrade : upgrades) {
        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
            if (upgrade == UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1 && base_count > 2) {
                TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
            }
        }
    //}
}

void researchTERRANINFANTRYWEAPONSLEVEL3(){
    auto upgrades = observation->GetUpgrades();
    //for (const auto& upgrade : upgrades) {
        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
            if (upgrade == UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2 && base_count > 2) {
                TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
            }
        }
    //}
}

void researchCONCUSSIVESHELLS(){
    auto upgrades = observation->GetUpgrades();
    if (upgrades.empty()) {
        TryBuildUnit(ABILITY_ID::RESEARCH_CONCUSSIVESHELLS, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
    }
}
void researchCOMBATSHIELD(){
    auto upgrades = observation->GetUpgrades();
    if (upgrades.empty()) {
        TryBuildUnit(ABILITY_ID::RESEARCH_COMBATSHIELD, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
    }
}
void researchSTIMPACK(){
    auto upgrades = observation->GetUpgrades();
    if (upgrades.empty()) {
        TryBuildUnit(ABILITY_ID::RESEARCH_STIMPACK, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
    }
}

void researchTERRANVEHICLEWEAPONSLEVEL1(){
    auto upgrades = observation->GetUpgrades();
    if (upgrades.empty()) {
        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
    }
    /*
    if (upgrade == UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1 && base_count > 2)
    {
        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
    }
    else if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1 && base_count > 2)
    {
        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
    }
    else if (upgrade == UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL1 && base_count > 2)
    {
        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::TERRAN_ARMORY);
    }
    else if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2 && base_count > 3)
    {
        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
    }
    else if (upgrade == UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL2 && base_count > 3)
    {
        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::ZERG_SPIRE);
    }
    */
}

void researchTERRANVEHICLEWEAPONSLEVEL2(){
    auto upgrades = observation->GetUpgrades();
    if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1) {
        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
    }
}

void researchTERRANVEHICLEWEAPONSLEVEL3(){
    auto upgrades = observation->GetUpgrades();
    if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2) {
        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
    }
}



//ECONOMY RULE

//COMBAT RULE -> RENAMED -> MILITARY RULE
/*
    const ObservationInterface* observation = Observation();
    //grab army and building counts
    Units barracks = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKS));
    Units factorys = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_FACTORY));
    Units starports = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_STARPORT));

    size_t widowmine_count = CountUnitTypeTotal(observation, widow_mine_types, UNIT_TYPEID::TERRAN_FACTORY, ABILITY_ID::TRAIN_WIDOWMINE);

    size_t hellbat_count = CountUnitTypeTotal(observation, hellion_types, UNIT_TYPEID::TERRAN_FACTORY, ABILITY_ID::TRAIN_HELLION);
    hellbat_count += CountUnitTypeBuilding(observation, UNIT_TYPEID::TERRAN_FACTORY, ABILITY_ID::TRAIN_HELLBAT);

    size_t siege_tank_count = CountUnitTypeTotal(observation, siege_tank_types, UNIT_TYPEID::TERRAN_FACTORY, ABILITY_ID::TRAIN_SIEGETANK);
    size_t viking_count = CountUnitTypeTotal(observation, viking_types, UNIT_TYPEID::TERRAN_FACTORY, ABILITY_ID::TRAIN_VIKINGFIGHTER);
    size_t marine_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_MARINE, UNIT_TYPEID::TERRAN_BARRACKS, ABILITY_ID::TRAIN_MARINE);
    size_t marauder_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_MARAUDER, UNIT_TYPEID::TERRAN_BARRACKS, ABILITY_ID::TRAIN_MARAUDER);
    size_t reaper_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_REAPER, UNIT_TYPEID::TERRAN_BARRACKS, ABILITY_ID::TRAIN_REAPER);
    size_t ghost_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_GHOST, UNIT_TYPEID::TERRAN_BARRACKS, ABILITY_ID::TRAIN_GHOST);
    size_t medivac_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_MEDIVAC, UNIT_TYPEID::TERRAN_STARPORT, ABILITY_ID::TRAIN_MEDIVAC);
    size_t raven_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_RAVEN, UNIT_TYPEID::TERRAN_STARPORT, ABILITY_ID::TRAIN_RAVEN);
    size_t battlecruiser_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_MEDIVAC, UNIT_TYPEID::TERRAN_STARPORT, ABILITY_ID::TRAIN_BATTLECRUISER);
    size_t banshee_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_MEDIVAC, UNIT_TYPEID::TERRAN_STARPORT, ABILITY_ID::TRAIN_BANSHEE);

*/
/*
void buildSTARPORTUNIT(){
    if (!starports.empty()) {
        for (const auto& starport : starports) {
            if (observation->GetUnit(starport->add_on_tag) == nullptr) {
                if (mech_build_) {
                    if (starport->orders.empty() && viking_count < 5) {
                        Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_VIKINGFIGHTER);
                    }
                }
                else {
                    if (starport->orders.empty() && medivac_count < 5) {
                        Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_MEDIVAC);
                    }
                }
                continue;
            }
            else {
                if (observation->GetUnit(starport->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_STARPORTREACTOR) {
                    if (mech_build_) {
                        if (starport->orders.size() < 2 && viking_count < 5) {
                            Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_VIKINGFIGHTER);
                        }
                    }
                    else {
                        if (starport->orders.size() < 2 && medivac_count < 5) {
                            Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_MEDIVAC);
                        }
                        if (starport->orders.size() < 2 && medivac_count < 3) {
                            Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_MEDIVAC);
                        }
                    }

                }
                else {
                    if (starport->orders.empty() && raven_count < 2) {
                        Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_RAVEN);
                    }
                    if (!mech_build_) {
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_FUSIONCORE) > 0) {
                            if (starport->orders.empty() && battlecruiser_count < 2) {
                                Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_BATTLECRUISER);
                                if (battlecruiser_count < 1) {
                                    return;
                                }
                            }
                        }
                        if (starport->orders.empty() && banshee_count < 2) {
                            Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_BANSHEE);
                        }
                    }
                }
            }
        }
    }
}
*/
void buildVIKING(){
    Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
    for (const auto& starport : starports) {
        if (observation->GetUnit(starport->add_on_tag) == nullptr) {
            if (starport->orders.empty() && viking_count < 5) {
                Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_VIKINGFIGHTER);
            }
        }else{
            if (observation->GetUnit(starport->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_STARPORTREACTOR) {
                if (starport->orders.size() < 2 && viking_count < 5) {
                    Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_VIKINGFIGHTER);
                }
            }
        }
    }
}

void buildMEDIVAC(){
    Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
    for (const auto& starport : starports) {
        if (observation->GetUnit(starport->add_on_tag) == nullptr) {
            if (starport->orders.empty() && medivac_count < 5) {
                Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_MEDIVAC);
            }
        }else{
            if (observation->GetUnit(starport->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_STARPORTREACTOR) {
                if (starport->orders.size() < 2 && medivac_count < 5) {
                    Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_MEDIVAC);
                }
            }
        }
    }
}


/*
        for (const auto& barrack : barracks) {
            if (observation->GetUnit(barrack->add_on_tag) == nullptr) {
                if (!mech_build_) {
                    if (barrack->orders.empty() && marine_count < 20) {
                        Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARINE);
                    }
                    else if (barrack->orders.empty() && observation->GetMinerals() > 1000 && observation->GetVespene() < 300) {
                        Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARINE);
                    }
                }
            }
            else {
                if (observation->GetUnit(barrack->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_BARRACKSREACTOR) {
                    if (mech_build_) {
                        if (barrack->orders.size() < 2 && marine_count < 5) {
                            Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARINE);
                        }
                    }
                    else{
                        if (barrack->orders.size() < 2 && marine_count < 20) {
                            Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARINE);
                        }
                        else if (observation->GetMinerals() > 1000 && observation->GetVespene() < 300) {
                            Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARINE);
                        }
                    }
                }
                else {
                    if (!mech_build_ && barrack->orders.empty()) {
                        if (reaper_count < 2) {
                            Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_REAPER);
                        }
                        else if (ghost_count < 4) {
                            Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_GHOST);
                        }
                        else if (marauder_count < 10) {
                            Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARAUDER);
                        }
                        else {
                            Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARINE);
                        }
                    }
                }
            }
        }
*/

void buildMARINE(){
    Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
    for (const auto& barrack : barracks) {
            if (observation->GetUnit(barrack->add_on_tag) == nullptr) {
                    if (barrack->orders.empty() && marine_count < 20) {
                        Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARINE);
                    }
            }
    }
}

void buildMARAUDER(){
    Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
    for (const auto& barrack : barracks) {
            if (observation->GetUnit(barrack->add_on_tag) == nullptr) {
                    if (barrack->orders.empty() && marauder_count < 20) {
                        Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARAUDER);
                    }
            }
    }
}
