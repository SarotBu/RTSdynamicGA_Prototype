//#include SOMETHING


//BUILD RULE
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
void researchTERRANINFANTRYWEAPONSLEVEL1(){
    auto upgrades = observation->GetUpgrades();
        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
            TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
        }
}
void researchTERRANINFANTRYWEAPONSLEVEL2(){
    auto upgrades = observation->GetUpgrades();
        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
            if (upgrade == UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1 && base_count > 2) {
                TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
            }
        }
}

void researchTERRANINFANTRYWEAPONSLEVEL3(){
    auto upgrades = observation->GetUpgrades();
        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
            if (upgrade == UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2 && base_count > 2) {
                TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
            }
        }
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
void buildVIKING(int param){
    Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
    for (const auto& starport : starports) {
        if (observation->GetUnit(starport->add_on_tag) == nullptr) {
            if (starport->orders.empty() && viking_count < param) {
                Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_VIKINGFIGHTER);
            }
        }else{
            if (observation->GetUnit(starport->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_STARPORTREACTOR) {
                if (starport->orders.size() < 2 && viking_count < param) {
                    Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_VIKINGFIGHTER);
                }
            }
        }
    }
}

void buildMEDIVAC(int param){
    Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
    for (const auto& starport : starports) {
        if (observation->GetUnit(starport->add_on_tag) == nullptr) {
            if (starport->orders.empty() && medivac_count < param) {
                Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_MEDIVAC);
            }
        }else{
            if (observation->GetUnit(starport->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_STARPORTREACTOR) {
                if (starport->orders.size() < 2 && medivac_count < param) {
                    Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_MEDIVAC);
                }
            }
        }
    }
}

void buildMARINE(int param){
    Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
    for (const auto& barrack : barracks) {
            if (observation->GetUnit(barrack->add_on_tag) == nullptr) {
                    if (barrack->orders.empty() && marine_count < param) {
                        Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARINE);
                    }
            }
    }
}

void buildMARAUDER(int param){
    Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
    for (const auto& barrack : barracks) {
            if (observation->GetUnit(barrack->add_on_tag) == nullptr) {
                    if (barrack->orders.empty() && marauder_count < param) {
                        Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARAUDER);
                    }
            }
    }
}

