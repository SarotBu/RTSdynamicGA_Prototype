#include "bot_examples.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <iterator>

#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"



void sc2::MainTerranBot::BuildOrder() {

	const ObservationInterface* observation = Observation();


	Units bases = observation->GetUnits(Unit::Self, IsTownHall());
	Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
	Units factorys = observation->GetUnits(Unit::Self, IsUnits(factory_types));
	Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
	Units barracks_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));
	Units factorys_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_FACTORYTECHLAB));
	Units starports_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_STARPORTTECHLAB));
	Units engbays = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_ENGINEERINGBAY));


	Units supply_depots = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_SUPPLYDEPOT));

	size_t base_count = observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size();

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

	/*
	if (barracks.size() < 6) buildBARRACK();
	buildBARRACKADDON();
	if (bases.size()>1) buildORBITALCOMMAND();
	if (factorys.size() < 6) buildFACTORY();
	buildFACTORYADDON();
	if (engbays.size() < 1) buildENGINEERINGBAY();
	if (starports.size() < 5) buildSTARPORT();
	//observation->GetResults();
	*/

}