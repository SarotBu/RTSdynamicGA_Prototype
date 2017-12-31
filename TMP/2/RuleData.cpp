//#include SOMETHING

#include "bot_examples.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <iterator>

#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"

/*
void sc2::MainTerranBot::sabu::build

void sc2::MainTerranBot::buildORBITALCOMMAND() {
	
	const sc2::ObservationInterface* observation = Observation();
	Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
	Units bases = observation->GetUnits(Unit::Self, IsTownHall());
	if (!barracks.empty()) {
		for (const auto& base : bases) {
			if (base->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER && observation->GetMinerals() > 150) {
				Actions()->UnitCommand(base, ABILITY_ID::MORPH_ORBITALCOMMAND);
			}
		}
	}
}
*/



namespace sc2 {
	
		//BUILD RULE
		bool MainTerranBot::buildORBITALCOMMAND() {
			const ObservationInterface* observation = Observation();
			Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
			Units bases = observation->GetUnits(Unit::Self, IsTownHall());
			if (!barracks.empty()) {
				for (const auto& base : bases) {
					if (base->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER && observation->GetMinerals() > 150) {
						Actions()->UnitCommand(base, ABILITY_ID::MORPH_ORBITALCOMMAND);
						return true;
					}
				}
			}
			return false;
		}
		
		bool MainTerranBot::buildBARRACKADDON() {
			const ObservationInterface* observation = Observation();
			Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
			Units barracks_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));
			for (const auto& barrack : barracks) {
				if (!barrack->orders.empty() || barrack->build_progress != 1) {
					continue;
				}
				if (observation->GetUnit(barrack->add_on_tag) == nullptr) {
					if (barracks_tech.size() < barracks.size() / 2 || barracks_tech.empty()) {
						return TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_BARRACKS, barrack->tag);
					}
					else {
						return TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_BARRACKS, barrack->tag);
					}
				}
			}
			return false;
		}

		bool MainTerranBot::buildBARRACKTECHLAB(){
			const ObservationInterface* observation = Observation();
			Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
			Units barracks_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));
			if (observation->GetMinerals() < 50 || observation->GetVespene() < 25) return false;
			for (const auto& barrack : barracks) {
				if (!barrack->orders.empty() || barrack->build_progress != 1) {
					continue;
				}
				if (observation->GetUnit(barrack->add_on_tag) == nullptr) {
					return TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_BARRACKS, barrack->tag);
				}
			}
			return false;
		}

		bool MainTerranBot::buildBARRACKREACTOR() {
			const ObservationInterface* observation = Observation();
			Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
			if (observation->GetMinerals() < 50 || observation->GetVespene() < 50) return false;
			for (const auto& barrack : barracks) {
				if (!barrack->orders.empty() || barrack->build_progress != 1) {
					continue;
				}
				if (observation->GetUnit(barrack->add_on_tag) == nullptr) {
					return TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_BARRACKS, barrack->tag);
				}
			}
			return false;
		}

		bool MainTerranBot::buildBARRACK() {
			const ObservationInterface* observation = Observation();
			if (observation->GetMinerals() <= 150) return false;
			Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
			size_t beforeBuilt = barracks.size();
			return TryBuildStructureRandom(ABILITY_ID::BUILD_BARRACKS, UNIT_TYPEID::TERRAN_SCV);
			
			//barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
			//return (barracks.size() > beforeBuilt);
		}

		bool MainTerranBot::buildFACTORY() {
			const ObservationInterface* observation = Observation();
			if (observation->GetMinerals() < 150 || observation->GetVespene() < 100) return false;
			return TryBuildStructureRandom(ABILITY_ID::BUILD_FACTORY, UNIT_TYPEID::TERRAN_SCV);
		}

		bool MainTerranBot::buildFACTORYADDON() {
			const ObservationInterface* observation = Observation();
			Units factorys = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_FACTORY));
			Units factorys_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_FACTORYTECHLAB));
			for (const auto& factory : factorys) {
				if (!factory->orders.empty()) {
					continue;
				}

				if (observation->GetUnit(factory->add_on_tag) == nullptr) {
					if (mech_build_) {
						if (factorys_tech.size() < factorys.size() / 2) {
							return TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_FACTORY, factory->tag);
						} else {
							return TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_FACTORY, factory->tag);
						}
					}
					else {
						if (CountUnitType(observation, UNIT_TYPEID::TERRAN_BARRACKSREACTOR) < 1) {
							return TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_FACTORY, factory->tag);
						} else {
							return TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_FACTORY, factory->tag);
						}
					}

				}
			}
			return false;
		}

		bool MainTerranBot::buildFACTORYTECHLAB() {
			const ObservationInterface* observation = Observation();
			if (observation->GetMinerals() < 50 || observation->GetVespene() < 25) return false;
			Units factorys = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_FACTORY));
			Units factorys_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_FACTORYTECHLAB));
			for (const auto& factory : factorys) {
				if (!factory->orders.empty()) {
					continue;
				}
				if (observation->GetUnit(factory->add_on_tag) == nullptr) {
					return TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_FACTORY, factory->tag);
				}
			}
			return false;
		}

		bool MainTerranBot::buildFACTORYREACTOR() {
			const ObservationInterface* observation = Observation();
			if (observation->GetMinerals() < 50 || observation->GetVespene() < 50) return false;
			Units factorys = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_FACTORY));
			for (const auto& factory : factorys) {
				if (!factory->orders.empty()) {
					continue;
				}
				if (observation->GetUnit(factory->add_on_tag) == nullptr) {
					return TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_FACTORY, factory->tag);
				}
			}
			return false;
		}


		bool MainTerranBot::buildSTARPORT() {
			const ObservationInterface* observation = Observation();
			if (observation->GetMinerals() < 150 || observation->GetVespene() < 100) return false;
			return TryBuildStructureRandom(ABILITY_ID::BUILD_STARPORT, UNIT_TYPEID::TERRAN_SCV);
		}

		bool MainTerranBot::buildSTARPORTADDON() {
			const ObservationInterface* observation = Observation();
			Units starports = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_STARPORT));
			for (const auto& starport : starports) {
				if (!starport->orders.empty()) {
					continue;
				}
				if (observation->GetUnit(starport->add_on_tag) == nullptr) {
					if (mech_build_) {
						if (CountUnitType(observation, UNIT_TYPEID::TERRAN_STARPORTREACTOR) < 2) {
							return TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_STARPORT, starport->tag);
						} else {
							return TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_STARPORT, starport->tag);
						}
					}
					else {
						if (CountUnitType(observation, UNIT_TYPEID::TERRAN_STARPORTREACTOR) < 1) {
							return TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_STARPORT, starport->tag);
						} else {
							return TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_STARPORT, starport->tag);
						}

					}
				}
			}
			return false;
		}

		bool MainTerranBot::buildSTARPORTTECHLAB() {
			const ObservationInterface* observation = Observation();
			Units starports = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_STARPORT));
			if (observation->GetMinerals() < 50 || observation->GetVespene() < 25) return false;
			for (const auto& starport : starports) {
				if (!starport->orders.empty()) {
					continue;
				}
				if (observation->GetUnit(starport->add_on_tag) == nullptr) {
					return TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_STARPORT, starport->tag);
				}
			}
			return false;
		}

		bool MainTerranBot::buildSTARPORTREACTOR() {
			const ObservationInterface* observation = Observation();
			Units starports = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_STARPORT));
			if (observation->GetMinerals() < 50 || observation->GetVespene() < 50) return false;
			for (const auto& starport : starports) {
				if (!starport->orders.empty()) {
					continue;
				}
				if (observation->GetUnit(starport->add_on_tag) == nullptr) {
					return TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_STARPORT, starport->tag);
				}
			}
			return false;
		}

		bool MainTerranBot::buildENGINEERINGBAY(){
			const ObservationInterface* observation = Observation();
			if (observation->GetMinerals() < 125) return false;
			return TryBuildStructureRandom(ABILITY_ID::BUILD_ENGINEERINGBAY, UNIT_TYPEID::TERRAN_SCV);
		}

		bool MainTerranBot::buildGHOSTACADEMY() {
			const ObservationInterface* observation = Observation();
			if (observation->GetMinerals() < 150 || observation->GetVespene() < 50) return false;
			return TryBuildStructureRandom(ABILITY_ID::BUILD_GHOSTACADEMY, UNIT_TYPEID::TERRAN_SCV);
		}

		bool MainTerranBot::buildARMORY() {
			const ObservationInterface* observation = Observation();
			if (observation->GetMinerals() < 150 || observation->GetVespene() < 100) return false;
			return TryBuildStructureRandom(ABILITY_ID::BUILD_ARMORY, UNIT_TYPEID::TERRAN_SCV);
		}

		bool MainTerranBot::buildFUSIONCORE() {
			const ObservationInterface* observation = Observation();
			if (observation->GetMinerals() < 150 || observation->GetVespene() < 150) return false;
			return TryBuildStructureRandom(ABILITY_ID::BUILD_FUSIONCORE, UNIT_TYPEID::TERRAN_SCV);
		}

		//RESEARCH RULE
		bool MainTerranBot::researchTERRANINFANTRYWEAPONSLEVEL1() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();
			if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
				return TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
			}
			return false;
		}

		bool MainTerranBot::researchTERRANINFANTRYWEAPONSLEVEL2() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();

			for (const auto& upgrade : upgrades) {
				if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
					if (upgrade == UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1) {
						return TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
					}
				}
			}
			return false;
		}

		bool MainTerranBot::researchTERRANINFANTRYWEAPONSLEVEL3() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();
			for (const auto& upgrade : upgrades) {
				if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
					if (upgrade == UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2) {
						return TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
					}
				}
			}
			return false;
		}

		bool MainTerranBot::researchTERRANINFANTRYARMORLEVEL1() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();
			if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
				return TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
			}
			return false;
		}

		bool MainTerranBot::researchTERRANINFANTRYARMORLEVEL2() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();

			for (const auto& upgrade : upgrades) {
				if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
					if (upgrade == UPGRADE_ID::TERRANINFANTRYARMORSLEVEL1) {
						return TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
					}
				}
			}
			return false;
		}

		bool MainTerranBot::researchTERRANINFANTRYARMORLEVEL3() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();

			for (const auto& upgrade : upgrades) {
				if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
					if (upgrade == UPGRADE_ID::TERRANINFANTRYARMORSLEVEL2) {
						return TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
					}
				}
			}
			return false;
		}

		bool MainTerranBot::researchCONCUSSIVESHELLS() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();
			if (upgrades.empty()) {
				return TryBuildUnit(ABILITY_ID::RESEARCH_CONCUSSIVESHELLS, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
			}
			return false;
		}
		bool MainTerranBot::researchCOMBATSHIELD() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();
			if (upgrades.empty()) {
				return TryBuildUnit(ABILITY_ID::RESEARCH_COMBATSHIELD, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
			}
			return false;
		}

		bool MainTerranBot::researchSTIMPACK() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();
			if (upgrades.empty()) {
				return TryBuildUnit(ABILITY_ID::RESEARCH_STIMPACK, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
			}
			return false;
		}

		bool MainTerranBot::researchTERRANVEHICLEWEAPONSLEVEL1() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();
			if (upgrades.empty()) {
				return TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
			}
			return false;
		}

		bool MainTerranBot::researchTERRANVEHICLEWEAPONSLEVEL2() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();
			for (const auto& upgrade : upgrades) {
				if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1) {
					return TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
				}
			}
			return false;
		}

		bool MainTerranBot::researchTERRANVEHICLEWEAPONSLEVEL3() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();
			for (const auto& upgrade : upgrades) {
				if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2) {
					return TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
				}
			}
			return false;
		}

		bool MainTerranBot::researchTERRANSHIPWEAPONSLEVEL1() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();
			if (upgrades.empty()) {
				return TryBuildUnit(ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
			}
			return false;
		}

		bool MainTerranBot::researchTERRANSHIPWEAPONSLEVEL2() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();
			for (const auto& upgrade : upgrades) {
				if (upgrade == UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1) {
					return TryBuildUnit(ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
				}
			}
			return false;
		}

		bool MainTerranBot::researchTERRANSHIPWEAPONSLEVEL3() {
			const ObservationInterface* observation = Observation();
			auto upgrades = observation->GetUpgrades();
			for (const auto& upgrade : upgrades) {
				if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2) {
					return TryBuildUnit(ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
				}
			}
			return false;
		}

		//ECONOMY RULE

		//COMBAT RULE -> RENAMED -> MILITARY RULE
		bool MainTerranBot::buildVIKING(int param) {
			const ObservationInterface* observation = Observation();
			Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
			size_t viking_count = CountUnitTypeTotal(observation, viking_types, UNIT_TYPEID::TERRAN_FACTORY, ABILITY_ID::TRAIN_VIKINGFIGHTER);
			int buildCnt = 0;
			for (const auto& starport : starports) {
				if (observation->GetUnit(starport->add_on_tag) == nullptr) {
					if (starport->orders.empty() && viking_count < param) {
						Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_VIKINGFIGHTER);
						buildCnt++;
					}
				}
				else {
					if (observation->GetUnit(starport->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_STARPORTREACTOR) {
						if (starport->orders.size() < 2 && viking_count < param) {
							Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_VIKINGFIGHTER);
							buildCnt++;
						}
					}
				}
			}
			return (buildCnt == param);
		}

		bool MainTerranBot::buildMEDIVAC(int param) {
			const ObservationInterface* observation = Observation();
			Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
			size_t medivac_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_MEDIVAC, UNIT_TYPEID::TERRAN_STARPORT, ABILITY_ID::TRAIN_MEDIVAC);
			int buildCnt = 0;
			for (const auto& starport : starports) {
				if (observation->GetUnit(starport->add_on_tag) == nullptr) {
					if (starport->orders.empty()) {
						Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_MEDIVAC);
						buildCnt++;
					}
				}
				else {
					if (observation->GetUnit(starport->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_STARPORTREACTOR) {
						if (starport->orders.size() < 2) {
							Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_MEDIVAC);
							buildCnt++;
						}
					}
				}
			}
			return (buildCnt == param);
		}

		bool MainTerranBot::buildMARINE(int param) {
			const ObservationInterface* observation = Observation();
			Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
			size_t buildCnt = 0;
			for (const auto& barrack : barracks) {
				if (barrack->build_progress < 1.0) continue;
				if (observation->GetUnit(barrack->add_on_tag) == nullptr) {
					if (barrack->orders.size() + param <= 5) {
						buildCnt += 5 - barrack->orders.size();
					}
				}else {
					if (observation->GetUnit(barrack->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_BARRACKSREACTOR) {
						if (barrack->orders.size() + param <= 8) {
							buildCnt += 8 - barrack->orders.size();
						}
					}else {
						if (barrack->orders.size() + param <= 5) {
							buildCnt += 5 - barrack->orders.size();
						}
					}
				}
			}
			if (buildCnt < param) return false;
			for (const auto& barrack : barracks) {
				if (barrack->build_progress < 1.0) continue;
				if (param <= 0) break;
				if (observation->GetUnit(barrack->add_on_tag) == nullptr) {
					if (barrack->orders.size() + param <= 5) {
						Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARINE);
						param--;
					}
				}
				else {
					if (observation->GetUnit(barrack->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_BARRACKSREACTOR) {
						if (barrack->orders.size() + param <= 8) {
							Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARINE);
							param--;
						}
					}
					else {
						if (barrack->orders.size() + param <= 5) {
							Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARINE);
							param--;
						}
					}
				}
			}
			return (param == 0);
		}

		bool MainTerranBot::buildMARAUDER(int param) {
			const ObservationInterface* observation = Observation();
			Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
			size_t marauder_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_MARAUDER, UNIT_TYPEID::TERRAN_BARRACKS, ABILITY_ID::TRAIN_MARAUDER);
			size_t buildCnt = 0;
			for (const auto& barrack : barracks) {
				if (barrack->build_progress < 1.0) continue;
				if (observation->GetUnit(barrack->add_on_tag) != nullptr) {
					if (observation->GetUnit(barrack->add_on_tag)->unit_type != UNIT_TYPEID::TERRAN_BARRACKSTECHLAB) continue;
				}
				if (barrack->orders.size() + param <= 5) {
					buildCnt += 5 - barrack->orders.size();
				}
			}
			if (buildCnt < param) return false;
			for (const auto& barrack : barracks) {
				if (barrack->build_progress < 1.0) continue;
				if (observation->GetUnit(barrack->add_on_tag) != nullptr) {
					if (observation->GetUnit(barrack->add_on_tag)->unit_type != UNIT_TYPEID::TERRAN_BARRACKSTECHLAB) continue;
				}
				while (barrack->orders.size() + 1 <= 5 && param > 0) {
					param--;
					Actions()->UnitCommand(barrack, ABILITY_ID::TRAIN_MARAUDER);
				}
			}
			return true;
		}

		bool MainTerranBot::buildHELLION(int param) {
			const ObservationInterface* observation = Observation();
			Units factorys = observation->GetUnits(Unit::Self, IsUnits(factory_types));
			size_t buildCnt = 0;
			if (!factorys.empty()) {
				for (const auto& factory : factorys) {
					if (factory->orders.empty()) {
						Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLION);
						buildCnt++;
					}
				}
			}
			return (buildCnt == param);
		}

		bool MainTerranBot::buildSIEGETANK(int param) {
			const ObservationInterface* observation = Observation();
			Units factorys = observation->GetUnits(Unit::Self, IsUnits(factory_types));
			int buildCnt = 0;
			if (!factorys.empty()) {
				for (const auto& factory : factorys) {
					if (factory->orders.empty()) {
						Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_SIEGETANK);
						buildCnt++;
					}
				}
			}
			return (buildCnt == param);
		}

		bool MainTerranBot::buildTHOR(int param) {
			const ObservationInterface* observation = Observation();
			Units factorys = observation->GetUnits(Unit::Self, IsUnits(factory_types));
			int buildCnt = 0;
			if (!factorys.empty()) {
				for (const auto& factory : factorys) {
					if (factory->orders.empty()) {
						Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_THOR);
						buildCnt++;
					}
				}
			}
			return (buildCnt == param);
		}

		bool MainTerranBot::buildBANSHEE(int param) {
			const ObservationInterface* observation = Observation();
			Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
			size_t banshee_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_BANSHEE, UNIT_TYPEID::TERRAN_STARPORT, ABILITY_ID::TRAIN_BANSHEE);
			int buildCnt = 0;
			for (const auto& starport : starports) {
				if (observation->GetUnit(starport->add_on_tag) == nullptr) {
					if (starport->orders.empty() && banshee_count < param) {
						Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_BANSHEE);
						buildCnt++;
					}
				}
				else {
					if (observation->GetUnit(starport->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_STARPORTREACTOR) {
						if (starport->orders.size() < 2 && banshee_count < param) {
							Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_BANSHEE);
							buildCnt++;
						}
					}
				}
			}
			return (buildCnt == param);
		}

}

