
#include "bot_examples.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <iterator>


//#include "sc2api/sc2_api.h"
//#include "sc2lib/sc2_lib.h"

namespace sc2 {
    
static int TargetSCVCount = 15;

struct IsVisible {
    bool operator()(const Unit& unit) { return unit.display_type == Unit::Visible; };
};
struct IsAttackable {
    bool operator()(const Unit& unit) {
        switch (unit.unit_type.ToType()) {
            case UNIT_TYPEID::ZERG_OVERLORD: return false;
            case UNIT_TYPEID::ZERG_OVERSEER: return false;
            case UNIT_TYPEID::PROTOSS_OBSERVER: return false;
            default: return true;
        }
    }
};

struct IsFlying {
    bool operator()(const Unit& unit) {
        return unit.is_flying;
    }
};

//Ignores Overlords, workers, and structures
struct IsArmy {
    IsArmy(const ObservationInterface* obs) : observation_(obs) {}

    bool operator()(const Unit& unit) {
        auto attributes = observation_->GetUnitTypeData().at(unit.unit_type).attributes;
        for (const auto& attribute : attributes) {
            if (attribute == Attribute::Structure) {
                return false;
            }
        }
        switch (unit.unit_type.ToType()) {
            case UNIT_TYPEID::ZERG_OVERLORD: return false;
            case UNIT_TYPEID::PROTOSS_PROBE: return false;
            case UNIT_TYPEID::ZERG_DRONE: return false;
            case UNIT_TYPEID::TERRAN_SCV: return false;
            case UNIT_TYPEID::ZERG_QUEEN: return false;
            case UNIT_TYPEID::ZERG_LARVA: return false;
            case UNIT_TYPEID::ZERG_EGG: return false;
            case UNIT_TYPEID::TERRAN_MULE: return false;
            case UNIT_TYPEID::TERRAN_NUKE: return false;
            default: return true;
        }
    }

    const ObservationInterface* observation_;
};

struct IsTownHall {
    bool operator()(const Unit& unit) {
        switch (unit.unit_type.ToType()) {
            case UNIT_TYPEID::ZERG_HATCHERY: return true;
            case UNIT_TYPEID::ZERG_LAIR: return true;
            case UNIT_TYPEID::ZERG_HIVE : return true;
            case UNIT_TYPEID::TERRAN_COMMANDCENTER: return true;
            case UNIT_TYPEID::TERRAN_ORBITALCOMMAND: return true;
            case UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING: return true;
            case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS: return true;
            case UNIT_TYPEID::PROTOSS_NEXUS: return true;
            default: return false;
        }
    }
};

struct IsVespeneGeyser {
    bool operator()(const Unit& unit) {
        switch (unit.unit_type.ToType()) {
            case UNIT_TYPEID::NEUTRAL_VESPENEGEYSER: return true;
            case UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER: return true;
            case UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER: return true;
            default: return false;
        }
    }
};

struct IsStructure {
    IsStructure(const ObservationInterface* obs) : observation_(obs) {};

    bool operator()(const Unit& unit) {
        auto& attributes = observation_->GetUnitTypeData().at(unit.unit_type).attributes;
        bool is_structure = false;
        for (const auto& attribute : attributes) {
            if (attribute == Attribute::Structure) {
                is_structure = true;
            }
        }
        return is_structure;
    }

    const ObservationInterface* observation_;
};

int CountUnitType(const ObservationInterface* observation, UnitTypeID unit_type) {
    int count = 0;
    Units my_units = observation->GetUnits(Unit::Alliance::Self);
    for (const auto unit : my_units) {
        if (unit->unit_type == unit_type)
            ++count;
    }

    return count;
}

bool FindEnemyStructure(const ObservationInterface* observation, const Unit*& enemy_unit) {
    Units my_units = observation->GetUnits(Unit::Alliance::Enemy);
    for (const auto unit : my_units) {
        if (unit->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER ||
            unit->unit_type == UNIT_TYPEID::TERRAN_SUPPLYDEPOT ||
            unit->unit_type == UNIT_TYPEID::TERRAN_BARRACKS) {
            enemy_unit = unit;
            return true;
        }
    }

    return false;
}

bool GetRandomUnit(const Unit*& unit_out, const ObservationInterface* observation, UnitTypeID unit_type) {
    Units my_units = observation->GetUnits(Unit::Alliance::Self);
    std::random_shuffle(my_units.begin(), my_units.end()); // Doesn't work, or doesn't work well.
    for (const auto unit : my_units) {
        if (unit->unit_type == unit_type) {
            unit_out = unit;
            return true;
        }
    }
    return false;
}

void MultiplayerBot::PrintStatus(std::string msg) {
    int64_t bot_identifier = int64_t(this) & 0xFFFLL;
    std::cout << std::to_string(bot_identifier) << ": " << msg << std::endl;
}

void MultiplayerBot::OnGameStart() {
    game_info_ = Observation()->GetGameInfo();
    PrintStatus("game started.");
    expansions_ = search::CalculateExpansionLocations(Observation(), Query());

    //Temporary, we can replace this with observation->GetStartLocation() once implemented
    startLocation_ = Observation()->GetStartLocation();
    staging_location_ = startLocation_;
};

size_t MultiplayerBot::CountUnitType(const ObservationInterface* observation, UnitTypeID unit_type) {
    return observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}

size_t MultiplayerBot::CountUnitTypeBuilding(const ObservationInterface* observation, UNIT_TYPEID production_building, ABILITY_ID ability) {
    int building_count = 0;
    Units buildings = observation->GetUnits(Unit::Self, IsUnit(production_building));

    for (const auto& building : buildings) {
        if (building->orders.empty()) {
            continue;
        }

        for (const auto order : building->orders) {
            if (order.ability_id == ability) {
                building_count++;
            }
        }
    }

    return building_count;
}

size_t MultiplayerBot::CountUnitTypeTotal(const ObservationInterface* observation, UNIT_TYPEID unit_type, UNIT_TYPEID production, ABILITY_ID ability) {
    return CountUnitType(observation, unit_type) + CountUnitTypeBuilding(observation, production, ability);
}

size_t MultiplayerBot::CountUnitTypeTotal(const ObservationInterface* observation, std::vector<UNIT_TYPEID> unit_type, UNIT_TYPEID production, ABILITY_ID ability) {
    size_t count = 0;
    for (const auto& type : unit_type) {
        count += CountUnitType(observation, type);
    }
    return count + CountUnitTypeBuilding(observation, production, ability);
}

bool MultiplayerBot::GetRandomUnit(const Unit*& unit_out, const ObservationInterface* observation, UnitTypeID unit_type) {
    Units my_units = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
    if (!my_units.empty()) {
        unit_out = GetRandomEntry(my_units);
        return true;
    }
    return false;
}

const Unit* MultiplayerBot::FindNearestMineralPatch(const Point2D& start) {
    Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
    float distance = std::numeric_limits<float>::max();
    const Unit* target = nullptr;
    for (const auto& u : units) {
        if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
            float d = DistanceSquared2D(u->pos, start);
            if (d < distance) {
                distance = d;
                target = u;
            }
        }
    }
    //If we never found one return false;
    if (distance == std::numeric_limits<float>::max()) {
        return target;
    }
    return target;
}

// Tries to find a random location that can be pathed to on the map.
// Returns 'true' if a new, random location has been found that is pathable by the unit.
bool MultiplayerBot::FindEnemyPosition(Point2D& target_pos) {
    if (game_info_.enemy_start_locations.empty()) {
        return false;
    }
    target_pos = game_info_.enemy_start_locations.front();
    return true;
}

bool MultiplayerBot::TryFindRandomPathableLocation(const Unit* unit, Point2D& target_pos) {
    // First, find a random point inside the playable area of the map.
    float playable_w = game_info_.playable_max.x - game_info_.playable_min.x;
    float playable_h = game_info_.playable_max.y - game_info_.playable_min.y;

    // The case where game_info_ does not provide a valid answer
    if (playable_w == 0 || playable_h == 0) {
        playable_w = 236;
        playable_h = 228;
    }

    target_pos.x = playable_w * GetRandomFraction() + game_info_.playable_min.x;
    target_pos.y = playable_h * GetRandomFraction() + game_info_.playable_min.y;

    // Now send a pathing query from the unit to that point. Can also query from point to point,
    // but using a unit tag wherever possible will be more accurate.
    // Note: This query must communicate with the game to get a result which affects performance.
    // Ideally batch up the queries (using PathingDistanceBatched) and do many at once.
    float distance = Query()->PathingDistance(unit, target_pos);

    return distance > 0.1f;
}

void MultiplayerBot::AttackWithUnitType(UnitTypeID unit_type, const ObservationInterface* observation) {
    Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
    for (const auto& unit : units) {
        AttackWithUnit(unit, observation);
    }
}

void MultiplayerBot::AttackWithUnit(const Unit* unit, const ObservationInterface* observation) {
    //If unit isn't doing anything make it attack.
    Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy);
    if (enemy_units.empty()) {
        return;
    }

    if (unit->orders.empty()) {
        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, enemy_units.front()->pos);
        return;
    }

    //If the unit is doing something besides attacking, make it attack.
    if (unit->orders.front().ability_id != ABILITY_ID::ATTACK) {
        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, enemy_units.front()->pos);
    }
}

void MultiplayerBot::ScoutWithUnits(UnitTypeID unit_type, const ObservationInterface* observation) {
    Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
    for (const auto& unit : units) {
        ScoutWithUnit(unit, observation);
    }
}

void MultiplayerBot::ScoutWithUnit(const Unit* unit, const ObservationInterface* observation) {
    Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy, IsAttackable());
    if (!unit->orders.empty()) {
        return;
    }
    Point2D target_pos;

    if (FindEnemyPosition(target_pos)) {
        if (Distance2D(unit->pos, target_pos) < 20 && enemy_units.empty()) {
            if (TryFindRandomPathableLocation(unit, target_pos)) {
                Actions()->UnitCommand(unit, ABILITY_ID::SMART, target_pos);
                return;
            }
        }
        else if (!enemy_units.empty())
        {
            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, enemy_units.front());
            return;
        }
        Actions()->UnitCommand(unit, ABILITY_ID::SMART, target_pos);
    }
    else {
        if (TryFindRandomPathableLocation(unit, target_pos)) {
            Actions()->UnitCommand(unit, ABILITY_ID::SMART, target_pos);
        }
    }
}

//Try build structure given a location. This is used most of the time
bool MultiplayerBot::TryBuildStructure(AbilityID ability_type_for_structure, UnitTypeID unit_type, Point2D location, bool isExpansion = false) {

    const ObservationInterface* observation = Observation();
    Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));

    //if we have no workers Don't build
    if (workers.empty()) {
        return false;
    }

    // Check to see if there is already a worker heading out to build it
    for (const auto& worker : workers) {
        for (const auto& order : worker->orders) {
            if (order.ability_id == ability_type_for_structure) {
                return false;
            }
        }
    }

    // If no worker is already building one, get a random worker to build one
    const Unit* unit = GetRandomEntry(workers);

    // Check to see if unit can make it there
    if (Query()->PathingDistance(unit, location) < 0.1f) {
        return false;
    }
    if (!isExpansion) {
        for (const auto& expansion : expansions_) {
            if (Distance2D(location, Point2D(expansion.x, expansion.y)) < 7) {
                return false;
            }
        }
    }
    // Check to see if unit can build there
    if (Query()->Placement(ability_type_for_structure, location)) {
        Actions()->UnitCommand(unit, ability_type_for_structure, location);
        return true;
    }
    return false;

}

//Try to build a structure based on tag, Used mostly for Vespene, since the pathing check will fail even though the geyser is "Pathable"
bool MultiplayerBot::TryBuildStructure(AbilityID ability_type_for_structure, UnitTypeID unit_type, Tag location_tag) {
    const ObservationInterface* observation = Observation();
    Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
    const Unit* target = observation->GetUnit(location_tag);

    if (workers.empty()) {
        return false;
    }

    // Check to see if there is already a worker heading out to build it
    for (const auto& worker : workers) {
        for (const auto& order : worker->orders) {
            if (order.ability_id == ability_type_for_structure) {
                return false;
            }
        }
    }

    // If no worker is already building one, get a random worker to build one
    const Unit* unit = GetRandomEntry(workers);

    // Check to see if unit can build there
    if (Query()->Placement(ability_type_for_structure, target->pos)) {
        Actions()->UnitCommand(unit, ability_type_for_structure, target);
        return true;
    }
    return false;

}

//Expands to nearest location and updates the start location to be between the new location and old bases.
bool MultiplayerBot::TryExpand(AbilityID build_ability, UnitTypeID worker_type) {
    const ObservationInterface* observation = Observation();
    float minimum_distance = std::numeric_limits<float>::max();
    Point3D closest_expansion;
    for (const auto& expansion : expansions_) {
        float current_distance = Distance2D(startLocation_, expansion);
        if (current_distance < .01f) {
            continue;
        }

        if (current_distance < minimum_distance) {
            if (Query()->Placement(build_ability, expansion)) {
                closest_expansion = expansion;
                minimum_distance = current_distance;
            }
        }
    }
    //only update staging location up till 3 bases.
    if (TryBuildStructure(build_ability, worker_type, closest_expansion, true) && observation->GetUnits(Unit::Self, IsTownHall()).size() < 4) {
        staging_location_ = Point3D(((staging_location_.x + closest_expansion.x) / 2), ((staging_location_.y + closest_expansion.y) / 2),
            ((staging_location_.z + closest_expansion.z) / 2));
        return true;
    }
    return false;

}

//Tries to build a geyser for a base
bool MultiplayerBot::TryBuildGas(AbilityID build_ability, UnitTypeID worker_type, Point2D base_location) {
    const ObservationInterface* observation = Observation();
    Units geysers = observation->GetUnits(Unit::Alliance::Neutral, IsVespeneGeyser());

    //only search within this radius
    float minimum_distance = 15.0f;
    Tag closestGeyser = 0;
    for (const auto& geyser : geysers) {
        float current_distance = Distance2D(base_location, geyser->pos);
        if (current_distance < minimum_distance) {
            if (Query()->Placement(build_ability, geyser->pos)) {
                minimum_distance = current_distance;
                closestGeyser = geyser->tag;
            }
        }
    }

    // In the case where there are no more available geysers nearby
    if (closestGeyser == 0) {
        return false;
    }
    return TryBuildStructure(build_ability, worker_type, closestGeyser);

}

bool MultiplayerBot::TryBuildUnit(AbilityID ability_type_for_unit, UnitTypeID unit_type) {
    const ObservationInterface* observation = Observation();

    //If we are at supply cap, don't build anymore units, unless its an overlord.
    if (observation->GetFoodUsed() >= observation->GetFoodCap() && ability_type_for_unit != ABILITY_ID::TRAIN_OVERLORD) {
        return false;
    }
    const Unit* unit = nullptr;
    if (!GetRandomUnit(unit, observation, unit_type)) {
        return false;
    }
    if (!unit->orders.empty()) {
        return false;
    }

    if (unit->build_progress != 1) {
        return false;
    }

    Actions()->UnitCommand(unit, ability_type_for_unit);
    return true;
}

// Mine the nearest mineral to Town hall.
// If we don't do this, probes may mine from other patches if they stray too far from the base after building.
void MultiplayerBot::MineIdleWorkers(const Unit* worker, AbilityID worker_gather_command, UnitTypeID vespene_building_type) {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    Units geysers = observation->GetUnits(Unit::Alliance::Self, IsUnit(vespene_building_type));

    const Unit* valid_mineral_patch = nullptr;

    if (bases.empty()) {
        return;
    }

    for (const auto& geyser : geysers) {
        if (geyser->assigned_harvesters < geyser->ideal_harvesters) {
            Actions()->UnitCommand(worker, worker_gather_command, geyser);
            return;
        }
    }
    //Search for a base that is missing workers.
    for (const auto& base : bases) {
        //If we have already mined out here skip the base.
        if (base->ideal_harvesters == 0 || base->build_progress != 1) {
            continue;
        }
        if (base->assigned_harvesters < base->ideal_harvesters) {
            valid_mineral_patch = FindNearestMineralPatch(base->pos);
            Actions()->UnitCommand(worker, worker_gather_command, valid_mineral_patch);
            return;
        }
    }

    if (!worker->orders.empty()) {
        return;
    }

    //If all workers are spots are filled just go to any base.
    const Unit* random_base = GetRandomEntry(bases);
    valid_mineral_patch = FindNearestMineralPatch(random_base->pos);
    Actions()->UnitCommand(worker, worker_gather_command, valid_mineral_patch);
}

//An estimate of how many workers we should have based on what buildings we have
int MultiplayerBot::GetExpectedWorkers(UNIT_TYPEID vespene_building_type) {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    Units geysers = observation->GetUnits(Unit::Alliance::Self, IsUnit(vespene_building_type));
    int expected_workers = 0;
    for (const auto& base : bases) {
        if (base->build_progress != 1) {
            continue;
        }
        expected_workers += base->ideal_harvesters;
    }

    for (const auto& geyser : geysers) {
        if (geyser->vespene_contents > 0) {
            if (geyser->build_progress != 1) {
                continue;
            }
            expected_workers += geyser->ideal_harvesters;
        }
    }

    return expected_workers;
}

// To ensure that we do not over or under saturate any base.
void MultiplayerBot::ManageWorkers(UNIT_TYPEID worker_type, AbilityID worker_gather_command, UNIT_TYPEID vespene_building_type) {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    Units geysers = observation->GetUnits(Unit::Alliance::Self, IsUnit(vespene_building_type));

    if (bases.empty()) {
        return;
    }

    for (const auto& base : bases) {
        //If we have already mined out or still building here skip the base.
        if (base->ideal_harvesters == 0 || base->build_progress != 1) {
            continue;
        }
        //if base is
        if (base->assigned_harvesters > base->ideal_harvesters) {
            Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(worker_type));

            for (const auto& worker : workers) {
                if (!worker->orders.empty()) {
                    if (worker->orders.front().target_unit_tag == base->tag) {
                        //This should allow them to be picked up by mineidleworkers()
                        MineIdleWorkers(worker, worker_gather_command,vespene_building_type);
                        return;
                    }
                }
            }
        }
    }
    Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(worker_type));
    for (const auto& geyser : geysers) {
        if (geyser->ideal_harvesters == 0 || geyser->build_progress != 1) {
            continue;
        }
        if (geyser->assigned_harvesters > geyser->ideal_harvesters) {
            for (const auto& worker : workers) {
                if (!worker->orders.empty()) {
                    if (worker->orders.front().target_unit_tag == geyser->tag) {
                        //This should allow them to be picked up by mineidleworkers()
                        MineIdleWorkers(worker, worker_gather_command, vespene_building_type);
                        return;
                    }
                }
            }
        }
        else if (geyser->assigned_harvesters < geyser->ideal_harvesters) {
            for (const auto& worker : workers) {
                if (!worker->orders.empty()) {
                    //This should move a worker that isn't mining gas to gas
                    const Unit* target = observation->GetUnit(worker->orders.front().target_unit_tag);
                    if (target == nullptr) {
                        continue;
                    }
                    if (target->unit_type != vespene_building_type) {
                        //This should allow them to be picked up by mineidleworkers()
                        MineIdleWorkers(worker, worker_gather_command, vespene_building_type);
                        return;
                    }
                }
            }
        }
    }
}

void MultiplayerBot::RetreatWithUnits(UnitTypeID unit_type, Point2D retreat_position) {
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
    for (const auto& unit : units) {
        RetreatWithUnit(unit, retreat_position);
    }
}

void MultiplayerBot::RetreatWithUnit(const Unit* unit, Point2D retreat_position) {
    float dist = Distance2D(unit->pos, retreat_position);

    if (dist < 10) {
        if (unit->orders.empty()) {
            return;
        }
        Actions()->UnitCommand(unit, ABILITY_ID::STOP);
        return;
    }

    if (unit->orders.empty() && dist > 14) {
        Actions()->UnitCommand(unit, ABILITY_ID::MOVE, retreat_position);
    }
    else if (!unit->orders.empty() && dist > 14) {
        if (unit->orders.front().ability_id != ABILITY_ID::MOVE) {
            Actions()->UnitCommand(unit, ABILITY_ID::MOVE, retreat_position);
        }
    }
}

void MultiplayerBot::OnNuclearLaunchDetected() {
    const ObservationInterface* observation = Observation();
    nuke_detected = true;
    nuke_detected_frame = observation->GetGameLoop();
}
/*
bool TerranMultiplayerBot::TryBuildSCV() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());

    for (const auto& base : bases) {
        if (base->unit_type == UNIT_TYPEID::TERRAN_ORBITALCOMMAND && base->energy > 50) {
            if (FindNearestMineralPatch(base->pos)) {
                Actions()->UnitCommand(base, ABILITY_ID::EFFECT_CALLDOWNMULE);
            }
        }
    }

    if (observation->GetFoodWorkers() >= max_worker_count_) {
        return false;
    }

    if (observation->GetFoodUsed() >= observation->GetFoodCap()) {
        return false;
    }

    if (observation->GetFoodWorkers() > GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY)) {
        return false;
    }

    for (const auto& base : bases) {
        //if there is a base with less than ideal workers
        if (base->assigned_harvesters < base->ideal_harvesters && base->build_progress == 1) {
            if (observation->GetMinerals() >= 50) {
                return TryBuildUnit(ABILITY_ID::TRAIN_SCV, base->unit_type);
            }
        }
    }
    return false;
}

bool TerranMultiplayerBot::TryBuildSupplyDepot()  {
    const ObservationInterface* observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() < observation->GetFoodCap() - 6) {
        return false;
    }

    if (observation->GetMinerals() < 100) {
        return false;
    }

    //check to see if there is already on building
    Units units = observation->GetUnits(Unit::Alliance::Self, IsUnits(supply_depot_types));
    if (observation->GetFoodUsed() < 40) {
        for (const auto& unit : units) {
            if (unit->build_progress != 1) {
                return false;
            }
        }
    }

    // Try and build a supply depot. Find a random SCV and give it the order.
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D build_location = Point2D(staging_location_.x + rx * 15, staging_location_.y + ry * 15);
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT, UNIT_TYPEID::TERRAN_SCV, build_location);
}

void TerranMultiplayerBot::BuildArmy() {
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

    if (!mech_build_ && CountUnitType(observation, UNIT_TYPEID::TERRAN_GHOSTACADEMY) + CountUnitType(observation, UNIT_TYPEID::TERRAN_FACTORY) > 0) {
        if (!nuke_built) {
            Units ghosts = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_GHOST));
            if (observation->GetMinerals() > 100 && observation->GetVespene() > 100) {
                TryBuildUnit(ABILITY_ID::BUILD_NUKE, UNIT_TYPEID::TERRAN_GHOSTACADEMY);
            }
            if (!ghosts.empty()) {
                AvailableAbilities abilities = Query()->GetAbilitiesForUnit(ghosts.front());
                for (const auto& ability : abilities.abilities) {
                    if (ability.ability_id == ABILITY_ID::EFFECT_NUKECALLDOWN) {
                        nuke_built = true;
                    }
                }
            }

        }
    }


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

    if (!barracks.empty()) {
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
    }

    if (!factorys.empty()) {
        for (const auto& factory : factorys) {
            if (observation->GetUnit(factory->add_on_tag) == nullptr) {
                if (mech_build_) {
                    if (factory->orders.empty() && hellbat_count < 20) {
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLBAT);
                        }
                        else {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLION);
                        }
                    }
                }
            }
            else {
                if (observation->GetUnit(factory->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_FACTORYREACTOR) {
                    if (factory->orders.size() < 2 && widowmine_count < 4) {
                        Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_WIDOWMINE);
                    }
                    if (mech_build_ && factory->orders.size() < 2) {
                        if (observation->GetMinerals() > 1000 && observation->GetVespene() < 300) {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLBAT);
                        }
                        if (hellbat_count < 20) {
                            if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                                Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLBAT);
                            }
                            else {
                                Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLION);
                            }
                        }
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_CYCLONE) < 6) {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_CYCLONE);
                        }
                    }
                }
                else {
                    if (mech_build_ && factory->orders.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_THOR) < 4) {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_THOR);
                            return;
                        }
                    }
                    if (!mech_build_ && factory->orders.empty() && siege_tank_count < 7) {
                        Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_SIEGETANK);
                    }
                    if (mech_build_ && factory->orders.empty() && siege_tank_count < 10) {
                        Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_SIEGETANK);
                    }
                }
            }
        }
    }
}

void TerranMultiplayerBot::BuildOrder() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Self, IsTownHall());
    Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
    Units factorys = observation->GetUnits(Unit::Self, IsUnits(factory_types));
    Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
    Units barracks_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));
    Units factorys_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_FACTORYTECHLAB));
    Units starports_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_STARPORTTECHLAB));

    Units supply_depots = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_SUPPLYDEPOT));
    if (bases.size() < 3 && CountUnitType(observation, UNIT_TYPEID::TERRAN_FUSIONCORE)) {
        TryBuildExpansionCom();
        return;
    }

    for (const auto& supply_depot : supply_depots) {
        Actions()->UnitCommand(supply_depot, ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER);
    }
    if (!barracks.empty()) {
        for (const auto& base : bases) {
            if (base->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER && observation->GetMinerals() > 150) {
                Actions()->UnitCommand(base, ABILITY_ID::MORPH_ORBITALCOMMAND);
            }
        }
    }

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

    for (const auto& starport : starports) {
        if (!starport->orders.empty()) {
            continue;
        }
        if (observation->GetUnit(starport->add_on_tag) == nullptr) {
            if (mech_build_) {
                if (CountUnitType(observation,UNIT_TYPEID::TERRAN_STARPORTREACTOR) < 2) {
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

    size_t barracks_count_target = std::min<size_t>(3 * bases.size(), 8);
    size_t factory_count_target = 1;
    size_t starport_count_target = 2;
    size_t armory_count_target = 1;
    if (mech_build_) {
        barracks_count_target = 1;
        armory_count_target = 2;
        factory_count_target = std::min<size_t>(2 * bases.size(), 7);
        starport_count_target = std::min<size_t>(1 * bases.size(), 4);
    }

    if (!factorys.empty() && starports.size() < std::min<size_t>(1 * bases.size(), 4)) {
        if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_STARPORT, UNIT_TYPEID::TERRAN_SCV);
        }
    }

    if (!barracks.empty() && factorys.size() < std::min<size_t>(2 * bases.size(), 7)) {
        if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_FACTORY, UNIT_TYPEID::TERRAN_SCV);
        }
    }

    if (barracks.size() < barracks_count_target) {
        if (observation->GetFoodWorkers() >= target_worker_count_) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_BARRACKS, UNIT_TYPEID::TERRAN_SCV);
        }
    }

    if (!mech_build_) {
        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ENGINEERINGBAY) < std::min<size_t>(bases.size(), 2)) {
            if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
                TryBuildStructureRandom(ABILITY_ID::BUILD_ENGINEERINGBAY, UNIT_TYPEID::TERRAN_SCV);
            }
        }
        if (!barracks.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_GHOSTACADEMY) < 1) {
            if (observation->GetMinerals() > 150 && observation->GetVespene() > 50) {
                TryBuildStructureRandom(ABILITY_ID::BUILD_GHOSTACADEMY, UNIT_TYPEID::TERRAN_SCV);
            }
        }
        if (!factorys.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_FUSIONCORE) < 1) {
            if (observation->GetMinerals() > 150 && observation->GetVespene() > 150) {
                TryBuildStructureRandom(ABILITY_ID::BUILD_FUSIONCORE, UNIT_TYPEID::TERRAN_SCV);
            }
        }
    }

    if (!barracks.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) < armory_count_target) {
        if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_ARMORY, UNIT_TYPEID::TERRAN_SCV);
        }
    }
}

bool TerranMultiplayerBot::TryBuildAddOn(AbilityID ability_type_for_structure, Tag base_structure) {
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    const Unit* unit = Observation()->GetUnit(base_structure);

    if (unit->build_progress != 1) {
        return false;
    }

    Point2D build_location = Point2D(unit->pos.x + rx * 15, unit->pos.y + ry * 15);
 
    Units units = Observation()->GetUnits(Unit::Self, IsStructure(Observation()));

    if (Query()->Placement(ability_type_for_structure, unit->pos, unit)) {
        Actions()->UnitCommand(unit, ability_type_for_structure);
        return true;
    }

    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        float d = Distance2D(u->pos, build_location);
        if (d < distance) {
            distance = d;
        }
    }
    if (distance < 6) {
        return false;
    }

    if(Query()->Placement(ability_type_for_structure, build_location, unit)){
        Actions()->UnitCommand(unit, ability_type_for_structure, build_location);
        return true;
    }
    return false;
    
}

bool TerranMultiplayerBot::TryBuildStructureRandom(AbilityID ability_type_for_structure, UnitTypeID unit_type) {
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D build_location = Point2D(staging_location_.x + rx * 15, staging_location_.y + ry * 15);

    Units units = Observation()->GetUnits(Unit::Self, IsStructure(Observation()));
    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        if (u->unit_type == UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED) {
            continue;
        }
        float d = Distance2D(u->pos, build_location);
        if (d < distance) {
            distance = d;
        }
    }
    if (distance < 6) {
        return false;
    }
    return TryBuildStructure(ability_type_for_structure, unit_type, build_location);
}

void TerranMultiplayerBot::ManageUpgrades() {
    const ObservationInterface* observation = Observation();
    auto upgrades = observation->GetUpgrades();
    size_t base_count = observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size();


    if (upgrades.empty()) {
        if (mech_build_) {
            TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
        }
        else {
            TryBuildUnit(ABILITY_ID::RESEARCH_STIMPACK, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
        }
    }
    else {
        for (const auto& upgrade : upgrades) {
            if (mech_build_) {
                if (upgrade == UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2 && base_count > 3) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL2 && base_count > 3) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::ZERG_SPIRE);
                }
                else {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::TERRAN_ARMORY);
                    TryBuildUnit(ABILITY_ID::RESEARCH_INFERNALPREIGNITER, UNIT_TYPEID::TERRAN_FACTORYTECHLAB);
                }
            }//Not mech build only
            else {
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
            }
        }
    }
}

void TerranMultiplayerBot::ManageArmy() {

    const ObservationInterface* observation = Observation();

    Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy);

    Units army = observation->GetUnits(Unit::Alliance::Self, IsArmy(observation));
    int wait_til_supply = 100;
    if (mech_build_) {
        wait_til_supply = 110;
    }

    Units nuke = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_NUKE));
    for (const auto& unit : army) {
         if (enemy_units.empty() && observation->GetFoodArmy() < wait_til_supply) {
            switch (unit->unit_type.ToType()) {
                case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: {
                    Actions()->UnitCommand(unit, ABILITY_ID::MORPH_UNSIEGE);
                    break;
                }
                default:
                    RetreatWithUnit(unit, staging_location_);
                    break;
            }
        }
        else if (!enemy_units.empty()) {
            switch (unit->unit_type.ToType()) {
                case UNIT_TYPEID::TERRAN_WIDOWMINE: {
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u->pos, unit->pos);
                        if (d < distance) {
                            distance = d;
                        }
                    }
                    if (distance < 6) {
                        Actions()->UnitCommand(unit, ABILITY_ID::BURROWDOWN);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_MARINE: {
                    if (stim_researched_ && !unit->orders.empty()) {
                        if (unit->orders.front().ability_id == ABILITY_ID::ATTACK) {
                            float distance = std::numeric_limits<float>::max();
                            for (const auto& u : enemy_units) {
                                float d = Distance2D(u->pos, unit->pos);
                                if (d < distance) {
                                    distance = d;
                                }
                            }
                            bool has_stimmed = false;
                            for (const auto& buff : unit->buffs) {
                                if (buff == BUFF_ID::STIMPACK) {
                                    has_stimmed = true;
                                }
                            }
                            if (distance < 6 && !has_stimmed) {
                                Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_STIM);
                                break;
                            }
                        }

                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_MARAUDER: {
                    if (stim_researched_ && !unit->orders.empty()) {
                        if (unit->orders.front().ability_id == ABILITY_ID::ATTACK) {
                            float distance = std::numeric_limits<float>::max();
                            for (const auto& u : enemy_units) {
                                float d = Distance2D(u->pos, unit->pos);
                                if (d < distance) {
                                    distance = d;
                                }
                            }
                            bool has_stimmed = false;
                            for (const auto& buff : unit->buffs) {
                                if (buff == BUFF_ID::STIMPACK) {
                                    has_stimmed = true;
                                }
                            }
                            if (distance < 7 && !has_stimmed) {
                                Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_STIM);
                                break;
                            }
                        }
                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_GHOST: {
                    float distance = std::numeric_limits<float>::max();
                    const Unit* closest_unit;
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u->pos, unit->pos);
                        if (d < distance) {
                            distance = d;
                            closest_unit = u;
                        }
                    }
                    if (ghost_cloak_researched_) {
                        if (distance < 7 && unit->energy > 50) {
                            Actions()->UnitCommand(unit, ABILITY_ID::BEHAVIOR_CLOAKON);
                            break;
                        }
                    }
                    if (nuke_built) {
                        Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_NUKECALLDOWN, closest_unit->pos);
                    }
                    else if (unit->energy > 50 && !unit->orders.empty()) {
                        if(unit->orders.front().ability_id == ABILITY_ID::ATTACK)
                        Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_GHOSTSNIPE, unit);
                        break;
                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_SIEGETANK: {
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u->pos, unit->pos);
                        if (d < distance) {
                            distance = d;
                        }
                    }
                    if (distance < 11) {
                        Actions()->UnitCommand(unit, ABILITY_ID::MORPH_SIEGEMODE);
                    }
                    else {
                        AttackWithUnit(unit, observation);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: {
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u->pos, unit->pos);
                        if (d < distance) {
                            distance = d;
                        }
                    }
                    if (distance > 13) {
                        Actions()->UnitCommand(unit, ABILITY_ID::MORPH_UNSIEGE);
                    }
                    else {
                        AttackWithUnit(unit, observation);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_MEDIVAC: {
                    Units bio_units = observation->GetUnits(Unit::Self, IsUnits(bio_types));
                    if (unit->orders.empty()) {
                        for (const auto& bio_unit : bio_units) {
                            if (bio_unit->health < bio_unit->health_max) {
                                Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_HEAL, bio_unit);
                                break;
                            }
                        }
                        if (!bio_units.empty()) {
                            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, bio_units.front());
                        }
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_VIKINGFIGHTER: {
                    Units flying_units = observation->GetUnits(Unit::Enemy, IsFlying());
                    if (flying_units.empty()) {
                        Actions()->UnitCommand(unit, ABILITY_ID::MORPH_VIKINGASSAULTMODE);
                    }
                    else {
                        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, flying_units.front());
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_VIKINGASSAULT: {
                    Units flying_units = observation->GetUnits(Unit::Enemy, IsFlying());
                    if (!flying_units.empty()) {
                        Actions()->UnitCommand(unit, ABILITY_ID::MORPH_VIKINGFIGHTERMODE);
                    }
                    else {
                        AttackWithUnit(unit, observation);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_CYCLONE: {
                    Units flying_units = observation->GetUnits(Unit::Enemy, IsFlying());
                    if (!flying_units.empty() && unit->orders.empty()) {
                        Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_LOCKON, flying_units.front());
                    }
                    else if (!flying_units.empty() && !unit->orders.empty()) {
                        if (unit->orders.front().ability_id != ABILITY_ID::EFFECT_LOCKON) {
                            Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_LOCKON, flying_units.front());
                        }
                    }
                    else {
                        AttackWithUnit(unit, observation);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_HELLION: {
                    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                        Actions()->UnitCommand(unit, ABILITY_ID::MORPH_HELLBAT);
                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_BANSHEE: {
                    if (banshee_cloak_researched_) {
                        float distance = std::numeric_limits<float>::max();
                        for (const auto& u : enemy_units) {
                            float d = Distance2D(u->pos, unit->pos);
                            if (d < distance) {
                                distance = d;
                            }
                        }
                        if (distance < 7 && unit->energy > 50) {
                            Actions()->UnitCommand(unit, ABILITY_ID::BEHAVIOR_CLOAKON);
                        }
                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_RAVEN: {
                    if (unit->energy > 125) {
                        Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_HUNTERSEEKERMISSILE, enemy_units.front());
                        break;
                    }
                    if (unit->orders.empty()) {
                        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, army.front()->pos);
                    }
                    break;
                }
                default: {
                    AttackWithUnit(unit, observation);
                }
                }
        }
        else {
            switch (unit->unit_type.ToType()) {
                case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: {
                    Actions()->UnitCommand(unit, ABILITY_ID::MORPH_UNSIEGE);
                    break;
                }
                case UNIT_TYPEID::TERRAN_MEDIVAC: {
                    Units bio_units = observation->GetUnits(Unit::Self, IsUnits(bio_types));
                    if (unit->orders.empty()) {
                        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, bio_units.front()->pos);
                    }
                    break;
                }
                default:
                    ScoutWithUnit(unit, observation);
                    break;
            }
        }
    }
}

bool TerranMultiplayerBot::TryBuildExpansionCom() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    //Don't have more active bases than we can provide workers for
    if (GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY) > max_worker_count_) {
        return false;
    }
    // If we have extra workers around, try and build another Hatch.
    if (GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY) < observation->GetFoodWorkers() - 10) {
        return TryExpand(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
    }
    //Only build another Hatch if we are floating extra minerals
    if (observation->GetMinerals() > std::min<size_t>(bases.size() * 400, 1200)) {
        return TryExpand(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
    }
    return false;
}

bool TerranMultiplayerBot::BuildRefinery() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());

    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_REFINERY) >= observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size() * 2) {
        return false;
    }

    for (const auto& base : bases) {
        if (base->assigned_harvesters >= base->ideal_harvesters) {
            if (base->build_progress == 1) {
                if (TryBuildGas(ABILITY_ID::BUILD_REFINERY, UNIT_TYPEID::TERRAN_SCV, base->pos)) {
                    return true;
                }
            }
        }
    }
    return false;
}
void TerranMultiplayerBot::OnStep() {


    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Self, IsArmy(observation));
    Units nukes = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_NUKE));

    //Throttle some behavior that can wait to avoid duplicate orders.
    int frames_to_skip = 4;
    if (observation->GetFoodUsed() >= observation->GetFoodCap()) {
        frames_to_skip = 6;
    }

    if (observation->GetGameLoop() % frames_to_skip != 0) {
        return;
    }

    if (!nuke_detected && nukes.empty()) {
        ManageArmy();
    }
    else {
        if (nuke_detected_frame + 400 < observation->GetGameLoop()) {
            nuke_detected = false;
        }
        for (const auto& unit : units) {
            RetreatWithUnit(unit, startLocation_);
        }
    }

    BuildOrder();

    ManageWorkers(UNIT_TYPEID::TERRAN_SCV, ABILITY_ID::HARVEST_GATHER, UNIT_TYPEID::TERRAN_REFINERY);

    ManageUpgrades();

    if (TryBuildSCV())
        return;

    if (TryBuildSupplyDepot())
        return;

    BuildArmy();

    if (BuildRefinery()) {
        return;
    }

    if (TryBuildExpansionCom()) {
        return;
    }
}

void TerranMultiplayerBot::OnUnitIdle(const Unit* unit) {
    switch (unit->unit_type.ToType()) {
        case UNIT_TYPEID::TERRAN_SCV: {
            MineIdleWorkers(unit, ABILITY_ID::HARVEST_GATHER, UNIT_TYPEID::TERRAN_REFINERY);
            break;
        }
        default:
            break;
    }
}

void TerranMultiplayerBot::OnUpgradeCompleted(UpgradeID upgrade) {
    switch (upgrade.ToType()) {
        case UPGRADE_ID::STIMPACK: {
            stim_researched_ = true;
        }
        case UPGRADE_ID::PERSONALCLOAKING: {
            ghost_cloak_researched_ = true;
        }
        case UPGRADE_ID::BANSHEECLOAK: {
            banshee_cloak_researched_ = true;
        }
        default:
            break;
    }
}

void TerranBot::PrintStatus(std::string msg) {
    int64_t bot_identifier = int64_t(this) & 0xFFFLL;
    std::cout << std::to_string(bot_identifier) << ": " << msg << std::endl;
}

void TerranBot::OnGameStart() {
    game_info_ = Observation()->GetGameInfo();
    PrintStatus("game started.");
}

// Tries to find a random location that can be pathed to on the map.
// Returns 'true' if a new, random location has been found that is pathable by the unit.
bool TerranBot::FindEnemyPosition(Point2D& target_pos) {
    if (game_info_.enemy_start_locations.empty()) return false;
    target_pos = game_info_.enemy_start_locations.front();
    return true;
}

void TerranBot::ScoutWithMarines() {

    Units units = Observation()->GetUnits(Unit::Alliance::Self);
    for (const auto& unit : units) {
        UnitTypeID unit_type(unit->unit_type);
        if (unit_type != UNIT_TYPEID::TERRAN_MARINE)
            continue;

        if (!unit->orders.empty())
            continue;

        // Priority to attacking enemy structures.
        const Unit* enemy_unit = nullptr;
        if (FindEnemyStructure(Observation(), enemy_unit)) {
            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, enemy_unit);
            return;
        }

        Point2D target_pos;
        // TODO: For efficiency, these queries should be batched.
        if (FindEnemyPosition(target_pos)) {
            Actions()->UnitCommand(unit, ABILITY_ID::SMART, target_pos);
        }
    }
}

bool TerranBot::TryBuildStructure(AbilityID ability_type_for_structure, UnitTypeID unit_type) {
    const ObservationInterface* observation = Observation();

    // If a unit already is building a supply structure of this type, do nothing.
    Units units = observation->GetUnits(Unit::Alliance::Self);
    for (const auto& unit : units) {
        for (const auto& order : unit->orders) {
            if (order.ability_id == ability_type_for_structure) {
                return false;
            }
        }
    }

    // Just try a random location near the unit.
    const Unit* unit = nullptr;
    if (!GetRandomUnit(unit, observation, unit_type))
        return false;

    float rx = GetRandomScalar();
    float ry = GetRandomScalar();

    Actions()->UnitCommand(unit, ability_type_for_structure, unit->pos + Point2D(rx, ry) * 15.0f);
    return true;
}

bool TerranBot::TryBuildSupplyDepot() {
    const ObservationInterface* observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
        return false;

    // Try and build a depot. Find a random TERRAN_SCV and give it the order.
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
}

bool TerranBot::TryBuildBarracks() {
    const ObservationInterface* observation = Observation();

    // Wait until we have our quota of TERRAN_SCV's.
    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_SCV) < TargetSCVCount)
        return false;

    // One build 1 barracks.
    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_BARRACKS) > 0)
        return false;

    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

bool TerranBot::TryBuildUnit(AbilityID ability_type_for_unit, UnitTypeID unit_type) {
    const ObservationInterface* observation = Observation();

    const Unit* unit = nullptr;
    if (!GetRandomUnit(unit, observation, unit_type))
        return false;

    if (!unit->orders.empty())
        return false;

    Actions()->UnitCommand(unit, ability_type_for_unit);
    return true;
}

bool TerranBot::TryBuildSCV() {
    if (CountUnitType(Observation(), UNIT_TYPEID::TERRAN_SCV) >= TargetSCVCount)
        return false;

    return TryBuildUnit(ABILITY_ID::TRAIN_SCV, UNIT_TYPEID::TERRAN_COMMANDCENTER);
}

bool TerranBot::TryBuildMarine() {
    return TryBuildUnit(ABILITY_ID::TRAIN_MARINE, UNIT_TYPEID::TERRAN_BARRACKS);
}

void TerranBot::OnStep() {
    // If there are marines and the command center is not found, send them scouting.
    ScoutWithMarines();

    // Build supply depots if they are needed.
    if (TryBuildSupplyDepot())
        return;

    // Build TERRAN_SCV's if they are needed.
    if (TryBuildSCV())
        return;

    // Build Barracks if they are ready to be built.
    if (TryBuildBarracks())
        return;

    // Just keep building marines if possible.
    if (TryBuildMarine())
        return;
}

void TerranBot::OnGameEnd() {
    std::cout << "Game Ended for: " << std::to_string(Control()->Proto().GetAssignedPort()) << std::endl;
}

void MarineMicroBot::OnGameStart() {
    move_back_ = false;
    targeted_zergling_ = 0;
}

void MarineMicroBot::OnStep() {
    const ObservationInterface* observation = Observation();
    ActionInterface* action = Actions();

    Point2D mp, zp;

    if (!GetPosition(UNIT_TYPEID::TERRAN_MARINE, Unit::Alliance::Self, mp)) {
        return;
    }

    if (!GetPosition(UNIT_TYPEID::ZERG_ZERGLING, Unit::Alliance::Enemy, zp)) {
        return;
    }

    if (!GetNearestZergling(mp)) {
        return;
    }

    Units units = observation->GetUnits(Unit::Alliance::Self);
    for (const auto& u : units) {
        switch (static_cast<UNIT_TYPEID>(u->unit_type)) {
            case UNIT_TYPEID::TERRAN_MARINE: {
                if (!move_back_) {
                    action->UnitCommand(u, ABILITY_ID::ATTACK, targeted_zergling_);
                }
                else {
                    if (Distance2D(mp, backup_target_) < 1.5f) {
                        move_back_ = false;
                    }

                    action->UnitCommand(u, ABILITY_ID::SMART, backup_target_);
                }
                break;
            }
            default: {
                break;
            }
        }
    }
}

void MarineMicroBot::OnUnitDestroyed(const Unit* unit) {
    if (unit == targeted_zergling_) {
        Point2D mp, zp;
        if (!GetPosition(UNIT_TYPEID::TERRAN_MARINE, Unit::Alliance::Self, mp)) {
            return;
        }

        if (!GetPosition(UNIT_TYPEID::ZERG_ZERGLING, Unit::Alliance::Enemy, zp)) {
            return;
        }

        Vector2D diff = mp - zp;
        Normalize2D(diff);

        targeted_zergling_ = 0;
        move_back_ = true;
        backup_start_ = mp;
        backup_target_ = mp + diff * 3.0f;
    }
}

bool MarineMicroBot::GetPosition(UNIT_TYPEID unit_type, Unit::Alliance alliace, Point2D& position) {
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(alliace);

    if (units.empty()) {
        return false;
    }

    position = Point2D(0.0f, 0.0f);
    unsigned int count = 0;

    for (const auto& u : units) {
        if (u->unit_type == unit_type) {
            position += u->pos;
            ++count;
        }
    }

    position /= (float)count;

    return true;
}

bool MarineMicroBot::GetNearestZergling(const Point2D& from) {
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Alliance::Enemy);

    if (units.empty()) {
        return false;
    }

    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        if (u->unit_type == UNIT_TYPEID::ZERG_ZERGLING) {
            float d = DistanceSquared2D(u->pos, from);
            if (d < distance) {
                distance = d;
                targeted_zergling_ = u;
            }
        }
    }

    return true;
}




bool TerranMultiplayerBot::TryBuildSCV() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());

    for (const auto& base : bases) {
        if (base->unit_type == UNIT_TYPEID::TERRAN_ORBITALCOMMAND && base->energy > 50) {
            if (FindNearestMineralPatch(base->pos)) {
                Actions()->UnitCommand(base, ABILITY_ID::EFFECT_CALLDOWNMULE);
            }
        }
    }

    if (observation->GetFoodWorkers() >= max_worker_count_) {
        return false;
    }

    if (observation->GetFoodUsed() >= observation->GetFoodCap()) {
        return false;
    }

    if (observation->GetFoodWorkers() > GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY)) {
        return false;
    }

    for (const auto& base : bases) {
        //if there is a base with less than ideal workers
        if (base->assigned_harvesters < base->ideal_harvesters && base->build_progress == 1) {
            if (observation->GetMinerals() >= 50) {
                return TryBuildUnit(ABILITY_ID::TRAIN_SCV, base->unit_type);
            }
        }
    }
    return false;
}

bool TerranMultiplayerBot::TryBuildSupplyDepot()  {
    const ObservationInterface* observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() < observation->GetFoodCap() - 6) {
        return false;
    }

    if (observation->GetMinerals() < 100) {
        return false;
    }

    //check to see if there is already on building
    Units units = observation->GetUnits(Unit::Alliance::Self, IsUnits(supply_depot_types));
    if (observation->GetFoodUsed() < 40) {
        for (const auto& unit : units) {
            if (unit->build_progress != 1) {
                return false;
            }
        }
    }

    // Try and build a supply depot. Find a random SCV and give it the order.
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D build_location = Point2D(staging_location_.x + rx * 15, staging_location_.y + ry * 15);
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT, UNIT_TYPEID::TERRAN_SCV, build_location);
}

void TerranMultiplayerBot::BuildArmy() {
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

    if (!mech_build_ && CountUnitType(observation, UNIT_TYPEID::TERRAN_GHOSTACADEMY) + CountUnitType(observation, UNIT_TYPEID::TERRAN_FACTORY) > 0) {
        if (!nuke_built) {
            Units ghosts = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_GHOST));
            if (observation->GetMinerals() > 100 && observation->GetVespene() > 100) {
                TryBuildUnit(ABILITY_ID::BUILD_NUKE, UNIT_TYPEID::TERRAN_GHOSTACADEMY);
            }
            if (!ghosts.empty()) {
                AvailableAbilities abilities = Query()->GetAbilitiesForUnit(ghosts.front());
                for (const auto& ability : abilities.abilities) {
                    if (ability.ability_id == ABILITY_ID::EFFECT_NUKECALLDOWN) {
                        nuke_built = true;
                    }
                }
            }

        }
    }


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

    if (!barracks.empty()) {
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
    }

    if (!factorys.empty()) {
        for (const auto& factory : factorys) {
            if (observation->GetUnit(factory->add_on_tag) == nullptr) {
                if (mech_build_) {
                    if (factory->orders.empty() && hellbat_count < 20) {
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLBAT);
                        }
                        else {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLION);
                        }
                    }
                }
            }
            else {
                if (observation->GetUnit(factory->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_FACTORYREACTOR) {
                    if (factory->orders.size() < 2 && widowmine_count < 4) {
                        Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_WIDOWMINE);
                    }
                    if (mech_build_ && factory->orders.size() < 2) {
                        if (observation->GetMinerals() > 1000 && observation->GetVespene() < 300) {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLBAT);
                        }
                        if (hellbat_count < 20) {
                            if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                                Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLBAT);
                            }
                            else {
                                Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLION);
                            }
                        }
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_CYCLONE) < 6) {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_CYCLONE);
                        }
                    }
                }
                else {
                    if (mech_build_ && factory->orders.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_THOR) < 4) {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_THOR);
                            return;
                        }
                    }
                    if (!mech_build_ && factory->orders.empty() && siege_tank_count < 7) {
                        Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_SIEGETANK);
                    }
                    if (mech_build_ && factory->orders.empty() && siege_tank_count < 10) {
                        Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_SIEGETANK);
                    }
                }
            }
        }
    }
}

void TerranMultiplayerBot::BuildOrder() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Self, IsTownHall());
    Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
    Units factorys = observation->GetUnits(Unit::Self, IsUnits(factory_types));
    Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
    Units barracks_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));
    Units factorys_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_FACTORYTECHLAB));
    Units starports_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_STARPORTTECHLAB));

    Units supply_depots = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_SUPPLYDEPOT));
    if (bases.size() < 3 && CountUnitType(observation, UNIT_TYPEID::TERRAN_FUSIONCORE)) {
        TryBuildExpansionCom();
        return;
    }

    for (const auto& supply_depot : supply_depots) {
        Actions()->UnitCommand(supply_depot, ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER);
    }
    if (!barracks.empty()) {
        for (const auto& base : bases) {
            if (base->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER && observation->GetMinerals() > 150) {
                Actions()->UnitCommand(base, ABILITY_ID::MORPH_ORBITALCOMMAND);
            }
        }
    }

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

    for (const auto& starport : starports) {
        if (!starport->orders.empty()) {
            continue;
        }
        if (observation->GetUnit(starport->add_on_tag) == nullptr) {
            if (mech_build_) {
                if (CountUnitType(observation,UNIT_TYPEID::TERRAN_STARPORTREACTOR) < 2) {
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

    size_t barracks_count_target = std::min<size_t>(3 * bases.size(), 8);
    size_t factory_count_target = 1;
    size_t starport_count_target = 2;
    size_t armory_count_target = 1;
    if (mech_build_) {
        barracks_count_target = 1;
        armory_count_target = 2;
        factory_count_target = std::min<size_t>(2 * bases.size(), 7);
        starport_count_target = std::min<size_t>(1 * bases.size(), 4);
    }

    if (!factorys.empty() && starports.size() < std::min<size_t>(1 * bases.size(), 4)) {
        if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_STARPORT, UNIT_TYPEID::TERRAN_SCV);
        }
    }

    if (!barracks.empty() && factorys.size() < std::min<size_t>(2 * bases.size(), 7)) {
        if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_FACTORY, UNIT_TYPEID::TERRAN_SCV);
        }
    }

    if (barracks.size() < barracks_count_target) {
        if (observation->GetFoodWorkers() >= target_worker_count_) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_BARRACKS, UNIT_TYPEID::TERRAN_SCV);
        }
    }

    if (!mech_build_) {
        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ENGINEERINGBAY) < std::min<size_t>(bases.size(), 2)) {
            if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
                TryBuildStructureRandom(ABILITY_ID::BUILD_ENGINEERINGBAY, UNIT_TYPEID::TERRAN_SCV);
            }
        }
        if (!barracks.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_GHOSTACADEMY) < 1) {
            if (observation->GetMinerals() > 150 && observation->GetVespene() > 50) {
                TryBuildStructureRandom(ABILITY_ID::BUILD_GHOSTACADEMY, UNIT_TYPEID::TERRAN_SCV);
            }
        }
        if (!factorys.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_FUSIONCORE) < 1) {
            if (observation->GetMinerals() > 150 && observation->GetVespene() > 150) {
                TryBuildStructureRandom(ABILITY_ID::BUILD_FUSIONCORE, UNIT_TYPEID::TERRAN_SCV);
            }
        }
    }

    if (!barracks.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) < armory_count_target) {
        if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_ARMORY, UNIT_TYPEID::TERRAN_SCV);
        }
    }
}

bool TerranMultiplayerBot::TryBuildAddOn(AbilityID ability_type_for_structure, Tag base_structure) {
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    const Unit* unit = Observation()->GetUnit(base_structure);

    if (unit->build_progress != 1) {
        return false;
    }

    Point2D build_location = Point2D(unit->pos.x + rx * 15, unit->pos.y + ry * 15);
 
    Units units = Observation()->GetUnits(Unit::Self, IsStructure(Observation()));

    if (Query()->Placement(ability_type_for_structure, unit->pos, unit)) {
        Actions()->UnitCommand(unit, ability_type_for_structure);
        return true;
    }

    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        float d = Distance2D(u->pos, build_location);
        if (d < distance) {
            distance = d;
        }
    }
    if (distance < 6) {
        return false;
    }

    if(Query()->Placement(ability_type_for_structure, build_location, unit)){
        Actions()->UnitCommand(unit, ability_type_for_structure, build_location);
        return true;
    }
    return false;
    
}

bool TerranMultiplayerBot::TryBuildStructureRandom(AbilityID ability_type_for_structure, UnitTypeID unit_type) {
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D build_location = Point2D(staging_location_.x + rx * 15, staging_location_.y + ry * 15);

    Units units = Observation()->GetUnits(Unit::Self, IsStructure(Observation()));
    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        if (u->unit_type == UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED) {
            continue;
        }
        float d = Distance2D(u->pos, build_location);
        if (d < distance) {
            distance = d;
        }
    }
    if (distance < 6) {
        return false;
    }
    return TryBuildStructure(ability_type_for_structure, unit_type, build_location);
}

void TerranMultiplayerBot::ManageUpgrades() {
    const ObservationInterface* observation = Observation();
    auto upgrades = observation->GetUpgrades();
    size_t base_count = observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size();


    if (upgrades.empty()) {
        if (mech_build_) {
            TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
        }
        else {
            TryBuildUnit(ABILITY_ID::RESEARCH_STIMPACK, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
        }
    }
    else {
        for (const auto& upgrade : upgrades) {
            if (mech_build_) {
                if (upgrade == UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2 && base_count > 3) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL2 && base_count > 3) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::ZERG_SPIRE);
                }
                else {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::TERRAN_ARMORY);
                    TryBuildUnit(ABILITY_ID::RESEARCH_INFERNALPREIGNITER, UNIT_TYPEID::TERRAN_FACTORYTECHLAB);
                }
            }//Not mech build only
            else {
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
            }
        }
    }
}

void TerranMultiplayerBot::ManageArmy() {

    const ObservationInterface* observation = Observation();

    Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy);

    Units army = observation->GetUnits(Unit::Alliance::Self, IsArmy(observation));
    int wait_til_supply = 100;
    if (mech_build_) {
        wait_til_supply = 110;
    }

    Units nuke = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_NUKE));
    for (const auto& unit : army) {
         if (enemy_units.empty() && observation->GetFoodArmy() < wait_til_supply) {
            switch (unit->unit_type.ToType()) {
                case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: {
                    Actions()->UnitCommand(unit, ABILITY_ID::MORPH_UNSIEGE);
                    break;
                }
                default:
                    RetreatWithUnit(unit, staging_location_);
                    break;
            }
        }
        else if (!enemy_units.empty()) {
            switch (unit->unit_type.ToType()) {
                case UNIT_TYPEID::TERRAN_WIDOWMINE: {
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u->pos, unit->pos);
                        if (d < distance) {
                            distance = d;
                        }
                    }
                    if (distance < 6) {
                        Actions()->UnitCommand(unit, ABILITY_ID::BURROWDOWN);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_MARINE: {
                    if (stim_researched_ && !unit->orders.empty()) {
                        if (unit->orders.front().ability_id == ABILITY_ID::ATTACK) {
                            float distance = std::numeric_limits<float>::max();
                            for (const auto& u : enemy_units) {
                                float d = Distance2D(u->pos, unit->pos);
                                if (d < distance) {
                                    distance = d;
                                }
                            }
                            bool has_stimmed = false;
                            for (const auto& buff : unit->buffs) {
                                if (buff == BUFF_ID::STIMPACK) {
                                    has_stimmed = true;
                                }
                            }
                            if (distance < 6 && !has_stimmed) {
                                Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_STIM);
                                break;
                            }
                        }

                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_MARAUDER: {
                    if (stim_researched_ && !unit->orders.empty()) {
                        if (unit->orders.front().ability_id == ABILITY_ID::ATTACK) {
                            float distance = std::numeric_limits<float>::max();
                            for (const auto& u : enemy_units) {
                                float d = Distance2D(u->pos, unit->pos);
                                if (d < distance) {
                                    distance = d;
                                }
                            }
                            bool has_stimmed = false;
                            for (const auto& buff : unit->buffs) {
                                if (buff == BUFF_ID::STIMPACK) {
                                    has_stimmed = true;
                                }
                            }
                            if (distance < 7 && !has_stimmed) {
                                Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_STIM);
                                break;
                            }
                        }
                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_GHOST: {
                    float distance = std::numeric_limits<float>::max();
                    const Unit* closest_unit;
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u->pos, unit->pos);
                        if (d < distance) {
                            distance = d;
                            closest_unit = u;
                        }
                    }
                    if (ghost_cloak_researched_) {
                        if (distance < 7 && unit->energy > 50) {
                            Actions()->UnitCommand(unit, ABILITY_ID::BEHAVIOR_CLOAKON);
                            break;
                        }
                    }
                    if (nuke_built) {
                        Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_NUKECALLDOWN, closest_unit->pos);
                    }
                    else if (unit->energy > 50 && !unit->orders.empty()) {
                        if(unit->orders.front().ability_id == ABILITY_ID::ATTACK)
                        Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_GHOSTSNIPE, unit);
                        break;
                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_SIEGETANK: {
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u->pos, unit->pos);
                        if (d < distance) {
                            distance = d;
                        }
                    }
                    if (distance < 11) {
                        Actions()->UnitCommand(unit, ABILITY_ID::MORPH_SIEGEMODE);
                    }
                    else {
                        AttackWithUnit(unit, observation);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: {
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u->pos, unit->pos);
                        if (d < distance) {
                            distance = d;
                        }
                    }
                    if (distance > 13) {
                        Actions()->UnitCommand(unit, ABILITY_ID::MORPH_UNSIEGE);
                    }
                    else {
                        AttackWithUnit(unit, observation);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_MEDIVAC: {
                    Units bio_units = observation->GetUnits(Unit::Self, IsUnits(bio_types));
                    if (unit->orders.empty()) {
                        for (const auto& bio_unit : bio_units) {
                            if (bio_unit->health < bio_unit->health_max) {
                                Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_HEAL, bio_unit);
                                break;
                            }
                        }
                        if (!bio_units.empty()) {
                            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, bio_units.front());
                        }
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_VIKINGFIGHTER: {
                    Units flying_units = observation->GetUnits(Unit::Enemy, IsFlying());
                    if (flying_units.empty()) {
                        Actions()->UnitCommand(unit, ABILITY_ID::MORPH_VIKINGASSAULTMODE);
                    }
                    else {
                        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, flying_units.front());
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_VIKINGASSAULT: {
                    Units flying_units = observation->GetUnits(Unit::Enemy, IsFlying());
                    if (!flying_units.empty()) {
                        Actions()->UnitCommand(unit, ABILITY_ID::MORPH_VIKINGFIGHTERMODE);
                    }
                    else {
                        AttackWithUnit(unit, observation);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_CYCLONE: {
                    Units flying_units = observation->GetUnits(Unit::Enemy, IsFlying());
                    if (!flying_units.empty() && unit->orders.empty()) {
                        Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_LOCKON, flying_units.front());
                    }
                    else if (!flying_units.empty() && !unit->orders.empty()) {
                        if (unit->orders.front().ability_id != ABILITY_ID::EFFECT_LOCKON) {
                            Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_LOCKON, flying_units.front());
                        }
                    }
                    else {
                        AttackWithUnit(unit, observation);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_HELLION: {
                    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                        Actions()->UnitCommand(unit, ABILITY_ID::MORPH_HELLBAT);
                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_BANSHEE: {
                    if (banshee_cloak_researched_) {
                        float distance = std::numeric_limits<float>::max();
                        for (const auto& u : enemy_units) {
                            float d = Distance2D(u->pos, unit->pos);
                            if (d < distance) {
                                distance = d;
                            }
                        }
                        if (distance < 7 && unit->energy > 50) {
                            Actions()->UnitCommand(unit, ABILITY_ID::BEHAVIOR_CLOAKON);
                        }
                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_RAVEN: {
                    if (unit->energy > 125) {
                        Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_HUNTERSEEKERMISSILE, enemy_units.front());
                        break;
                    }
                    if (unit->orders.empty()) {
                        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, army.front()->pos);
                    }
                    break;
                }
                default: {
                    AttackWithUnit(unit, observation);
                }
                }
        }
        else {
            switch (unit->unit_type.ToType()) {
                case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: {
                    Actions()->UnitCommand(unit, ABILITY_ID::MORPH_UNSIEGE);
                    break;
                }
                case UNIT_TYPEID::TERRAN_MEDIVAC: {
                    Units bio_units = observation->GetUnits(Unit::Self, IsUnits(bio_types));
                    if (unit->orders.empty()) {
                        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, bio_units.front()->pos);
                    }
                    break;
                }
                default:
                    ScoutWithUnit(unit, observation);
                    break;
            }
        }
    }
}

bool TerranMultiplayerBot::TryBuildExpansionCom() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    //Don't have more active bases than we can provide workers for
    if (GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY) > max_worker_count_) {
        return false;
    }
    // If we have extra workers around, try and build another Hatch.
    if (GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY) < observation->GetFoodWorkers() - 10) {
        return TryExpand(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
    }
    //Only build another Hatch if we are floating extra minerals
    if (observation->GetMinerals() > std::min<size_t>(bases.size() * 400, 1200)) {
        return TryExpand(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
    }
    return false;
}

bool TerranMultiplayerBot::BuildRefinery() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());

    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_REFINERY) >= observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size() * 2) {
        return false;
    }

    for (const auto& base : bases) {
        if (base->assigned_harvesters >= base->ideal_harvesters) {
            if (base->build_progress == 1) {
                if (TryBuildGas(ABILITY_ID::BUILD_REFINERY, UNIT_TYPEID::TERRAN_SCV, base->pos)) {
                    return true;
                }
            }
        }
    }
    return false;
}
void TerranMultiplayerBot::OnStep() {


    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Self, IsArmy(observation));
    Units nukes = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_NUKE));

    //Throttle some behavior that can wait to avoid duplicate orders.
    int frames_to_skip = 4;
    if (observation->GetFoodUsed() >= observation->GetFoodCap()) {
        frames_to_skip = 6;
    }

    if (observation->GetGameLoop() % frames_to_skip != 0) {
        return;
    }

    if (!nuke_detected && nukes.empty()) {
        ManageArmy();
    }
    else {
        if (nuke_detected_frame + 400 < observation->GetGameLoop()) {
            nuke_detected = false;
        }
        for (const auto& unit : units) {
            RetreatWithUnit(unit, startLocation_);
        }
    }

    BuildOrder();

    ManageWorkers(UNIT_TYPEID::TERRAN_SCV, ABILITY_ID::HARVEST_GATHER, UNIT_TYPEID::TERRAN_REFINERY);

    ManageUpgrades();

    if (TryBuildSCV())
        return;

    if (TryBuildSupplyDepot())
        return;

    BuildArmy();

    if (BuildRefinery()) {
        return;
    }

    if (TryBuildExpansionCom()) {
        return;
    }
}

void TerranMultiplayerBot::OnUnitIdle(const Unit* unit) {
    switch (unit->unit_type.ToType()) {
        case UNIT_TYPEID::TERRAN_SCV: {
            MineIdleWorkers(unit, ABILITY_ID::HARVEST_GATHER, UNIT_TYPEID::TERRAN_REFINERY);
            break;
        }
        default:
            break;
    }
}

void TerranMultiplayerBot::OnUpgradeCompleted(UpgradeID upgrade) {
    switch (upgrade.ToType()) {
        case UPGRADE_ID::STIMPACK: {
            stim_researched_ = true;
        }
        case UPGRADE_ID::PERSONALCLOAKING: {
            ghost_cloak_researched_ = true;
        }
        case UPGRADE_ID::BANSHEECLOAK: {
            banshee_cloak_researched_ = true;
        }
        default:
            break;
    }
}

void TerranBot::PrintStatus(std::string msg) {
    int64_t bot_identifier = int64_t(this) & 0xFFFLL;
    std::cout << std::to_string(bot_identifier) << ": " << msg << std::endl;
}

void TerranBot::OnGameStart() {
    game_info_ = Observation()->GetGameInfo();
    PrintStatus("game started.");
}

// Tries to find a random location that can be pathed to on the map.
// Returns 'true' if a new, random location has been found that is pathable by the unit.
bool TerranBot::FindEnemyPosition(Point2D& target_pos) {
    if (game_info_.enemy_start_locations.empty()) return false;
    target_pos = game_info_.enemy_start_locations.front();
    return true;
}

void TerranBot::ScoutWithMarines() {

    Units units = Observation()->GetUnits(Unit::Alliance::Self);
    for (const auto& unit : units) {
        UnitTypeID unit_type(unit->unit_type);
        if (unit_type != UNIT_TYPEID::TERRAN_MARINE)
            continue;

        if (!unit->orders.empty())
            continue;

        // Priority to attacking enemy structures.
        const Unit* enemy_unit = nullptr;
        if (FindEnemyStructure(Observation(), enemy_unit)) {
            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, enemy_unit);
            return;
        }

        Point2D target_pos;
        // TODO: For efficiency, these queries should be batched.
        if (FindEnemyPosition(target_pos)) {
            Actions()->UnitCommand(unit, ABILITY_ID::SMART, target_pos);
        }
    }
}

bool TerranBot::TryBuildStructure(AbilityID ability_type_for_structure, UnitTypeID unit_type) {
    const ObservationInterface* observation = Observation();

    // If a unit already is building a supply structure of this type, do nothing.
    Units units = observation->GetUnits(Unit::Alliance::Self);
    for (const auto& unit : units) {
        for (const auto& order : unit->orders) {
            if (order.ability_id == ability_type_for_structure) {
                return false;
            }
        }
    }

    // Just try a random location near the unit.
    const Unit* unit = nullptr;
    if (!GetRandomUnit(unit, observation, unit_type))
        return false;

    float rx = GetRandomScalar();
    float ry = GetRandomScalar();

    Actions()->UnitCommand(unit, ability_type_for_structure, unit->pos + Point2D(rx, ry) * 15.0f);
    return true;
}

bool TerranBot::TryBuildSupplyDepot() {
    const ObservationInterface* observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
        return false;

    // Try and build a depot. Find a random TERRAN_SCV and give it the order.
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
}

bool TerranBot::TryBuildBarracks() {
    const ObservationInterface* observation = Observation();

    // Wait until we have our quota of TERRAN_SCV's.
    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_SCV) < TargetSCVCount)
        return false;

    // One build 1 barracks.
    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_BARRACKS) > 0)
        return false;

    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

bool TerranBot::TryBuildUnit(AbilityID ability_type_for_unit, UnitTypeID unit_type) {
    const ObservationInterface* observation = Observation();

    const Unit* unit = nullptr;
    if (!GetRandomUnit(unit, observation, unit_type))
        return false;

    if (!unit->orders.empty())
        return false;

    Actions()->UnitCommand(unit, ability_type_for_unit);
    return true;
}

bool TerranBot::TryBuildSCV() {
    if (CountUnitType(Observation(), UNIT_TYPEID::TERRAN_SCV) >= TargetSCVCount)
        return false;

    return TryBuildUnit(ABILITY_ID::TRAIN_SCV, UNIT_TYPEID::TERRAN_COMMANDCENTER);
}

bool TerranBot::TryBuildMarine() {
    return TryBuildUnit(ABILITY_ID::TRAIN_MARINE, UNIT_TYPEID::TERRAN_BARRACKS);
}

void TerranBot::OnStep() {
    // If there are marines and the command center is not found, send them scouting.
    ScoutWithMarines();

    // Build supply depots if they are needed.
    if (TryBuildSupplyDepot())
        return;

    // Build TERRAN_SCV's if they are needed.
    if (TryBuildSCV())
        return;

    // Build Barracks if they are ready to be built.
    if (TryBuildBarracks())
        return;

    // Just keep building marines if possible.
    if (TryBuildMarine())
        return;
}

void TerranBot::OnGameEnd() {
    std::cout << "Game Ended for: " << std::to_string(Control()->Proto().GetAssignedPort()) << std::endl;
}

void MarineMicroBot::OnGameStart() {
    move_back_ = false;
    targeted_zergling_ = 0;
}

void MarineMicroBot::OnStep() {
    const ObservationInterface* observation = Observation();
    ActionInterface* action = Actions();

    Point2D mp, zp;

    if (!GetPosition(UNIT_TYPEID::TERRAN_MARINE, Unit::Alliance::Self, mp)) {
        return;
    }

    if (!GetPosition(UNIT_TYPEID::ZERG_ZERGLING, Unit::Alliance::Enemy, zp)) {
        return;
    }

    if (!GetNearestZergling(mp)) {
        return;
    }

    Units units = observation->GetUnits(Unit::Alliance::Self);
    for (const auto& u : units) {
        switch (static_cast<UNIT_TYPEID>(u->unit_type)) {
            case UNIT_TYPEID::TERRAN_MARINE: {
                if (!move_back_) {
                    action->UnitCommand(u, ABILITY_ID::ATTACK, targeted_zergling_);
                }
                else {
                    if (Distance2D(mp, backup_target_) < 1.5f) {
                        move_back_ = false;
                    }

                    action->UnitCommand(u, ABILITY_ID::SMART, backup_target_);
                }
                break;
            }
            default: {
                break;
            }
        }
    }
}

void MarineMicroBot::OnUnitDestroyed(const Unit* unit) {
    if (unit == targeted_zergling_) {
        Point2D mp, zp;
        if (!GetPosition(UNIT_TYPEID::TERRAN_MARINE, Unit::Alliance::Self, mp)) {
            return;
        }

        if (!GetPosition(UNIT_TYPEID::ZERG_ZERGLING, Unit::Alliance::Enemy, zp)) {
            return;
        }

        Vector2D diff = mp - zp;
        Normalize2D(diff);

        targeted_zergling_ = 0;
        move_back_ = true;
        backup_start_ = mp;
        backup_target_ = mp + diff * 3.0f;
    }
}

bool MarineMicroBot::GetPosition(UNIT_TYPEID unit_type, Unit::Alliance alliace, Point2D& position) {
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(alliace);

    if (units.empty()) {
        return false;
    }

    position = Point2D(0.0f, 0.0f);
    unsigned int count = 0;

    for (const auto& u : units) {
        if (u->unit_type == unit_type) {
            position += u->pos;
            ++count;
        }
    }

    position /= (float)count;

    return true;
}

bool MarineMicroBot::GetNearestZergling(const Point2D& from) {
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Alliance::Enemy);

    if (units.empty()) {
        return false;
    }

    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        if (u->unit_type == UNIT_TYPEID::ZERG_ZERGLING) {
            float d = DistanceSquared2D(u->pos, from);
            if (d < distance) {
                distance = d;
                targeted_zergling_ = u;
            }
        }
    }

    return true;
}
*/
/*



























*/

bool SaBuBot::TryBuildSCV() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());

    for (const auto& base : bases) {
        if (base->unit_type == UNIT_TYPEID::TERRAN_ORBITALCOMMAND && base->energy > 50) {
            if (FindNearestMineralPatch(base->pos)) {
                Actions()->UnitCommand(base, ABILITY_ID::EFFECT_CALLDOWNMULE);
            }
        }
    }

    if (observation->GetFoodWorkers() >= max_worker_count_) {
        return false;
    }

    if (observation->GetFoodUsed() >= observation->GetFoodCap()) {
        return false;
    }

    if (observation->GetFoodWorkers() > GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY)) {
        return false;
    }

    for (const auto& base : bases) {
        //if there is a base with less than ideal workers
        if (base->assigned_harvesters < base->ideal_harvesters && base->build_progress == 1) {
            if (observation->GetMinerals() >= 50) {
                return TryBuildUnit(ABILITY_ID::TRAIN_SCV, base->unit_type);
            }
        }
    }
    return false;
}

bool SaBuBot::TryBuildSupplyDepot() {
    const ObservationInterface* observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() < observation->GetFoodCap() - 6) {
        return false;
    }

    if (observation->GetMinerals() < 100) {
        return false;
    }

    //check to see if there is already on building
    Units units = observation->GetUnits(Unit::Alliance::Self, IsUnits(supply_depot_types));
    if (observation->GetFoodUsed() < 40) {
        for (const auto& unit : units) {
            if (unit->build_progress != 1) {
                return false;
            }
        }
    }

    // Try and build a supply depot. Find a random SCV and give it the order.
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D build_location = Point2D(staging_location_.x + rx * 15, staging_location_.y + ry * 15);
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT, UNIT_TYPEID::TERRAN_SCV, build_location);
}

void SaBuBot::BuildArmy() {
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

    if (!mech_build_ && CountUnitType(observation, UNIT_TYPEID::TERRAN_GHOSTACADEMY) + CountUnitType(observation, UNIT_TYPEID::TERRAN_FACTORY) > 0) {
        if (!nuke_built) {
            Units ghosts = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_GHOST));
            if (observation->GetMinerals() > 100 && observation->GetVespene() > 100) {
                TryBuildUnit(ABILITY_ID::BUILD_NUKE, UNIT_TYPEID::TERRAN_GHOSTACADEMY);
            }
            if (!ghosts.empty()) {
                AvailableAbilities abilities = Query()->GetAbilitiesForUnit(ghosts.front());
                for (const auto& ability : abilities.abilities) {
                    if (ability.ability_id == ABILITY_ID::EFFECT_NUKECALLDOWN) {
                        nuke_built = true;
                    }
                }
            }

        }
    }


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

    if (!barracks.empty()) {
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
                    else {
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
    }

    if (!factorys.empty()) {
        for (const auto& factory : factorys) {
            if (observation->GetUnit(factory->add_on_tag) == nullptr) {
                if (mech_build_) {
                    if (factory->orders.empty() && hellbat_count < 20) {
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLBAT);
                        }
                        else {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLION);
                        }
                    }
                }
            }
            else {
                if (observation->GetUnit(factory->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_FACTORYREACTOR) {
                    if (factory->orders.size() < 2 && widowmine_count < 4) {
                        Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_WIDOWMINE);
                    }
                    if (mech_build_ && factory->orders.size() < 2) {
                        if (observation->GetMinerals() > 1000 && observation->GetVespene() < 300) {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLBAT);
                        }
                        if (hellbat_count < 20) {
                            if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                                Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLBAT);
                            }
                            else {
                                Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLION);
                            }
                        }
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_CYCLONE) < 6) {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_CYCLONE);
                        }
                    }
                }
                else {
                    if (mech_build_ && factory->orders.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_THOR) < 4) {
                            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_THOR);
                            return;
                        }
                    }
                    if (!mech_build_ && factory->orders.empty() && siege_tank_count < 7) {
                        Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_SIEGETANK);
                    }
                    if (mech_build_ && factory->orders.empty() && siege_tank_count < 10) {
                        Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_SIEGETANK);
                    }
                }
            }
        }
    }
}

void SaBuBot::BuildOrder() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Self, IsTownHall());
    Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
    Units factorys = observation->GetUnits(Unit::Self, IsUnits(factory_types));
    Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
    Units barracks_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));
    Units factorys_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_FACTORYTECHLAB));
    Units starports_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_STARPORTTECHLAB));

    Units supply_depots = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_SUPPLYDEPOT));
    if (bases.size() < 3 && CountUnitType(observation, UNIT_TYPEID::TERRAN_FUSIONCORE)) {
        TryBuildExpansionCom();
        return;
    }

    for (const auto& supply_depot : supply_depots) {
        Actions()->UnitCommand(supply_depot, ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER);
    }
    if (!barracks.empty()) {
        for (const auto& base : bases) {
            if (base->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER && observation->GetMinerals() > 150) {
                Actions()->UnitCommand(base, ABILITY_ID::MORPH_ORBITALCOMMAND);
            }
        }
    }

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

    size_t barracks_count_target = std::min<size_t>(3 * bases.size(), 8);
    size_t factory_count_target = 1;
    size_t starport_count_target = 2;
    size_t armory_count_target = 1;
    if (mech_build_) {
        barracks_count_target = 1;
        armory_count_target = 2;
        factory_count_target = std::min<size_t>(2 * bases.size(), 7);
        starport_count_target = std::min<size_t>(1 * bases.size(), 4);
    }

    if (!factorys.empty() && starports.size() < std::min<size_t>(1 * bases.size(), 4)) {
        if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_STARPORT, UNIT_TYPEID::TERRAN_SCV);
        }
    }

    if (!barracks.empty() && factorys.size() < std::min<size_t>(2 * bases.size(), 7)) {
        if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_FACTORY, UNIT_TYPEID::TERRAN_SCV);
        }
    }

    if (barracks.size() < barracks_count_target) {
        if (observation->GetFoodWorkers() >= target_worker_count_) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_BARRACKS, UNIT_TYPEID::TERRAN_SCV);
        }
    }

    if (!mech_build_) {
        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ENGINEERINGBAY) < std::min<size_t>(bases.size(), 2)) {
            if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
                TryBuildStructureRandom(ABILITY_ID::BUILD_ENGINEERINGBAY, UNIT_TYPEID::TERRAN_SCV);
            }
        }
        if (!barracks.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_GHOSTACADEMY) < 1) {
            if (observation->GetMinerals() > 150 && observation->GetVespene() > 50) {
                TryBuildStructureRandom(ABILITY_ID::BUILD_GHOSTACADEMY, UNIT_TYPEID::TERRAN_SCV);
            }
        }
        if (!factorys.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_FUSIONCORE) < 1) {
            if (observation->GetMinerals() > 150 && observation->GetVespene() > 150) {
                TryBuildStructureRandom(ABILITY_ID::BUILD_FUSIONCORE, UNIT_TYPEID::TERRAN_SCV);
            }
        }
    }

    if (!barracks.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) < armory_count_target) {
        if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_ARMORY, UNIT_TYPEID::TERRAN_SCV);
        }
    }
}

bool SaBuBot::TryBuildAddOn(AbilityID ability_type_for_structure, Tag base_structure) {
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    const Unit* unit = Observation()->GetUnit(base_structure);

    if (unit->build_progress != 1) {
        return false;
    }

    Point2D build_location = Point2D(unit->pos.x + rx * 15, unit->pos.y + ry * 15);

    Units units = Observation()->GetUnits(Unit::Self, IsStructure(Observation()));

    if (Query()->Placement(ability_type_for_structure, unit->pos, unit)) {
        Actions()->UnitCommand(unit, ability_type_for_structure);
        return true;
    }

    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        float d = Distance2D(u->pos, build_location);
        if (d < distance) {
            distance = d;
        }
    }
    if (distance < 6) {
        return false;
    }

    if (Query()->Placement(ability_type_for_structure, build_location, unit)) {
        Actions()->UnitCommand(unit, ability_type_for_structure, build_location);
        return true;
    }
    return false;

}

bool SaBuBot::TryBuildStructureRandom(AbilityID ability_type_for_structure, UnitTypeID unit_type) {
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D build_location = Point2D(staging_location_.x + rx * 15, staging_location_.y + ry * 15);

    Units units = Observation()->GetUnits(Unit::Self, IsStructure(Observation()));
    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        if (u->unit_type == UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED) {
            continue;
        }
        float d = Distance2D(u->pos, build_location);
        if (d < distance) {
            distance = d;
        }
    }
    if (distance < 6) {
        return false;
    }
    return TryBuildStructure(ability_type_for_structure, unit_type, build_location);
}

void SaBuBot::ManageUpgrades() {
    const ObservationInterface* observation = Observation();
    auto upgrades = observation->GetUpgrades();
    size_t base_count = observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size();


    if (upgrades.empty()) {
        if (mech_build_) {
            TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
        }
        else {
            TryBuildUnit(ABILITY_ID::RESEARCH_STIMPACK, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
        }
    }
    else {
        for (const auto& upgrade : upgrades) {
            if (mech_build_) {
                if (upgrade == UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2 && base_count > 3) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL2 && base_count > 3) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::ZERG_SPIRE);
                }
                else {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::TERRAN_ARMORY);
                    TryBuildUnit(ABILITY_ID::RESEARCH_INFERNALPREIGNITER, UNIT_TYPEID::TERRAN_FACTORYTECHLAB);
                }
            }//Not mech build only
            else {
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
            }
        }
    }
}

void SaBuBot::ManageArmy() {

    const ObservationInterface* observation = Observation();

    Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy);

    Units army = observation->GetUnits(Unit::Alliance::Self, IsArmy(observation));
    int wait_til_supply = 100;
    if (mech_build_) {
        wait_til_supply = 110;
    }

    Units nuke = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_NUKE));
    for (const auto& unit : army) {
        if (enemy_units.empty() && observation->GetFoodArmy() < wait_til_supply) {
            switch (unit->unit_type.ToType()) {
            case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: {
                Actions()->UnitCommand(unit, ABILITY_ID::MORPH_UNSIEGE);
                break;
            }
            default:
                RetreatWithUnit(unit, staging_location_);
                break;
            }
        }
        else if (!enemy_units.empty()) {
            switch (unit->unit_type.ToType()) {
            case UNIT_TYPEID::TERRAN_WIDOWMINE: {
                float distance = std::numeric_limits<float>::max();
                for (const auto& u : enemy_units) {
                    float d = Distance2D(u->pos, unit->pos);
                    if (d < distance) {
                        distance = d;
                    }
                }
                if (distance < 6) {
                    Actions()->UnitCommand(unit, ABILITY_ID::BURROWDOWN);
                }
                break;
            }
            case UNIT_TYPEID::TERRAN_MARINE: {
                if (stim_researched_ && !unit->orders.empty()) {
                    if (unit->orders.front().ability_id == ABILITY_ID::ATTACK) {
                        float distance = std::numeric_limits<float>::max();
                        for (const auto& u : enemy_units) {
                            float d = Distance2D(u->pos, unit->pos);
                            if (d < distance) {
                                distance = d;
                            }
                        }
                        bool has_stimmed = false;
                        for (const auto& buff : unit->buffs) {
                            if (buff == BUFF_ID::STIMPACK) {
                                has_stimmed = true;
                            }
                        }
                        if (distance < 6 && !has_stimmed) {
                            Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_STIM);
                            break;
                        }
                    }

                }
                AttackWithUnit(unit, observation);
                break;
            }
            case UNIT_TYPEID::TERRAN_MARAUDER: {
                if (stim_researched_ && !unit->orders.empty()) {
                    if (unit->orders.front().ability_id == ABILITY_ID::ATTACK) {
                        float distance = std::numeric_limits<float>::max();
                        for (const auto& u : enemy_units) {
                            float d = Distance2D(u->pos, unit->pos);
                            if (d < distance) {
                                distance = d;
                            }
                        }
                        bool has_stimmed = false;
                        for (const auto& buff : unit->buffs) {
                            if (buff == BUFF_ID::STIMPACK) {
                                has_stimmed = true;
                            }
                        }
                        if (distance < 7 && !has_stimmed) {
                            Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_STIM);
                            break;
                        }
                    }
                }
                AttackWithUnit(unit, observation);
                break;
            }
            case UNIT_TYPEID::TERRAN_GHOST: {
                float distance = std::numeric_limits<float>::max();
                const Unit* closest_unit;
                for (const auto& u : enemy_units) {
                    float d = Distance2D(u->pos, unit->pos);
                    if (d < distance) {
                        distance = d;
                        closest_unit = u;
                    }
                }
                if (ghost_cloak_researched_) {
                    if (distance < 7 && unit->energy > 50) {
                        Actions()->UnitCommand(unit, ABILITY_ID::BEHAVIOR_CLOAKON);
                        break;
                    }
                }
                if (nuke_built) {
                    Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_NUKECALLDOWN, closest_unit->pos);
                }
                else if (unit->energy > 50 && !unit->orders.empty()) {
                    if (unit->orders.front().ability_id == ABILITY_ID::ATTACK)
                        Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_GHOSTSNIPE, unit);
                    break;
                }
                AttackWithUnit(unit, observation);
                break;
            }
            case UNIT_TYPEID::TERRAN_SIEGETANK: {
                float distance = std::numeric_limits<float>::max();
                for (const auto& u : enemy_units) {
                    float d = Distance2D(u->pos, unit->pos);
                    if (d < distance) {
                        distance = d;
                    }
                }
                if (distance < 11) {
                    Actions()->UnitCommand(unit, ABILITY_ID::MORPH_SIEGEMODE);
                }
                else {
                    AttackWithUnit(unit, observation);
                }
                break;
            }
            case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: {
                float distance = std::numeric_limits<float>::max();
                for (const auto& u : enemy_units) {
                    float d = Distance2D(u->pos, unit->pos);
                    if (d < distance) {
                        distance = d;
                    }
                }
                if (distance > 13) {
                    Actions()->UnitCommand(unit, ABILITY_ID::MORPH_UNSIEGE);
                }
                else {
                    AttackWithUnit(unit, observation);
                }
                break;
            }
            case UNIT_TYPEID::TERRAN_MEDIVAC: {
                Units bio_units = observation->GetUnits(Unit::Self, IsUnits(bio_types));
                if (unit->orders.empty()) {
                    for (const auto& bio_unit : bio_units) {
                        if (bio_unit->health < bio_unit->health_max) {
                            Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_HEAL, bio_unit);
                            break;
                        }
                    }
                    if (!bio_units.empty()) {
                        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, bio_units.front());
                    }
                }
                break;
            }
            case UNIT_TYPEID::TERRAN_VIKINGFIGHTER: {
                Units flying_units = observation->GetUnits(Unit::Enemy, IsFlying());
                if (flying_units.empty()) {
                    Actions()->UnitCommand(unit, ABILITY_ID::MORPH_VIKINGASSAULTMODE);
                }
                else {
                    Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, flying_units.front());
                }
                break;
            }
            case UNIT_TYPEID::TERRAN_VIKINGASSAULT: {
                Units flying_units = observation->GetUnits(Unit::Enemy, IsFlying());
                if (!flying_units.empty()) {
                    Actions()->UnitCommand(unit, ABILITY_ID::MORPH_VIKINGFIGHTERMODE);
                }
                else {
                    AttackWithUnit(unit, observation);
                }
                break;
            }
            case UNIT_TYPEID::TERRAN_CYCLONE: {
                Units flying_units = observation->GetUnits(Unit::Enemy, IsFlying());
                if (!flying_units.empty() && unit->orders.empty()) {
                    Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_LOCKON, flying_units.front());
                }
                else if (!flying_units.empty() && !unit->orders.empty()) {
                    if (unit->orders.front().ability_id != ABILITY_ID::EFFECT_LOCKON) {
                        Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_LOCKON, flying_units.front());
                    }
                }
                else {
                    AttackWithUnit(unit, observation);
                }
                break;
            }
            case UNIT_TYPEID::TERRAN_HELLION: {
                if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                    Actions()->UnitCommand(unit, ABILITY_ID::MORPH_HELLBAT);
                }
                AttackWithUnit(unit, observation);
                break;
            }
            case UNIT_TYPEID::TERRAN_BANSHEE: {
                if (banshee_cloak_researched_) {
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u->pos, unit->pos);
                        if (d < distance) {
                            distance = d;
                        }
                    }
                    if (distance < 7 && unit->energy > 50) {
                        Actions()->UnitCommand(unit, ABILITY_ID::BEHAVIOR_CLOAKON);
                    }
                }
                AttackWithUnit(unit, observation);
                break;
            }
            case UNIT_TYPEID::TERRAN_RAVEN: {
                if (unit->energy > 125) {
                    Actions()->UnitCommand(unit, ABILITY_ID::EFFECT_HUNTERSEEKERMISSILE, enemy_units.front());
                    break;
                }
                if (unit->orders.empty()) {
                    Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, army.front()->pos);
                }
                break;
            }
            default: {
                AttackWithUnit(unit, observation);
            }
            }
        }
        else {
            switch (unit->unit_type.ToType()) {
            case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: {
                Actions()->UnitCommand(unit, ABILITY_ID::MORPH_UNSIEGE);
                break;
            }
            case UNIT_TYPEID::TERRAN_MEDIVAC: {
                Units bio_units = observation->GetUnits(Unit::Self, IsUnits(bio_types));
                if (unit->orders.empty()) {
                    Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, bio_units.front()->pos);
                }
                break;
            }
            default:
                ScoutWithUnit(unit, observation);
                break;
            }
        }
    }
}

bool SaBuBot::TryBuildExpansionCom() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    //Don't have more active bases than we can provide workers for
    if (GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY) > max_worker_count_) {
        return false;
    }
    // If we have extra workers around, try and build another Hatch.
    if (GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY) < observation->GetFoodWorkers() - 10) {
        return TryExpand(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
    }
    //Only build another Hatch if we are floating extra minerals
    if (observation->GetMinerals() > std::min<size_t>(bases.size() * 400, 1200)) {
        return TryExpand(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
    }
    return false;
}

bool SaBuBot::BuildRefinery() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());

    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_REFINERY) >= observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size() * 2) {
        return false;
    }

    for (const auto& base : bases) {
        if (base->assigned_harvesters >= base->ideal_harvesters) {
            if (base->build_progress == 1) {
                if (TryBuildGas(ABILITY_ID::BUILD_REFINERY, UNIT_TYPEID::TERRAN_SCV, base->pos)) {
                    return true;
                }
            }
        }
    }
    return false;
}
void SaBuBot::OnStep() {


    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Self, IsArmy(observation));
    Units nukes = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_NUKE));

    //Throttle some behavior that can wait to avoid duplicate orders.
    int frames_to_skip = 4;
    if (observation->GetFoodUsed() >= observation->GetFoodCap()) {
        frames_to_skip = 6;
    }

    if (observation->GetGameLoop() % frames_to_skip != 0) {
        return;
    }

    if (!nuke_detected && nukes.empty()) {
        ManageArmy();
    }
    else {
        if (nuke_detected_frame + 400 < observation->GetGameLoop()) {
            nuke_detected = false;
        }
        for (const auto& unit : units) {
            RetreatWithUnit(unit, startLocation_);
        }
    }

    BuildOrder();

    ManageWorkers(UNIT_TYPEID::TERRAN_SCV, ABILITY_ID::HARVEST_GATHER, UNIT_TYPEID::TERRAN_REFINERY);

    ManageUpgrades();

    if (TryBuildSCV())
        return;

    if (TryBuildSupplyDepot())
        return;

    BuildArmy();

    if (BuildRefinery()) {
        return;
    }

    if (TryBuildExpansionCom()) {
        return;
    }
}

void SaBuBot::OnUnitIdle(const Unit* unit) {
    switch (unit->unit_type.ToType()) {
    case UNIT_TYPEID::TERRAN_SCV: {
        MineIdleWorkers(unit, ABILITY_ID::HARVEST_GATHER, UNIT_TYPEID::TERRAN_REFINERY);
        break;
    }
    default:
        break;
    }
}

void SaBuBot::OnUpgradeCompleted(UpgradeID upgrade) {
    switch (upgrade.ToType()) {
    case UPGRADE_ID::STIMPACK: {
        stim_researched_ = true;
    }
    case UPGRADE_ID::PERSONALCLOAKING: {
        ghost_cloak_researched_ = true;
    }
    case UPGRADE_ID::BANSHEECLOAK: {
        banshee_cloak_researched_ = true;
    }
    default:
        break;
    }
}
/*
void TerranBot::PrintStatus(std::string msg) {
    int64_t bot_identifier = int64_t(this) & 0xFFFLL;
    std::cout << std::to_string(bot_identifier) << ": " << msg << std::endl;
}

void TerranBot::OnGameStart() {
    game_info_ = Observation()->GetGameInfo();
    PrintStatus("game started.");
}

// Tries to find a random location that can be pathed to on the map.
// Returns 'true' if a new, random location has been found that is pathable by the unit.
bool TerranBot::FindEnemyPosition(Point2D& target_pos) {
    if (game_info_.enemy_start_locations.empty()) return false;
    target_pos = game_info_.enemy_start_locations.front();
    return true;
}

void TerranBot::ScoutWithMarines() {

    Units units = Observation()->GetUnits(Unit::Alliance::Self);
    for (const auto& unit : units) {
        UnitTypeID unit_type(unit->unit_type);
        if (unit_type != UNIT_TYPEID::TERRAN_MARINE)
            continue;

        if (!unit->orders.empty())
            continue;

        // Priority to attacking enemy structures.
        const Unit* enemy_unit = nullptr;
        if (FindEnemyStructure(Observation(), enemy_unit)) {
            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, enemy_unit);
            return;
        }

        Point2D target_pos;
        // TODO: For efficiency, these queries should be batched.
        if (FindEnemyPosition(target_pos)) {
            Actions()->UnitCommand(unit, ABILITY_ID::SMART, target_pos);
        }
    }
}

bool TerranBot::TryBuildStructure(AbilityID ability_type_for_structure, UnitTypeID unit_type) {
    const ObservationInterface* observation = Observation();

    // If a unit already is building a supply structure of this type, do nothing.
    Units units = observation->GetUnits(Unit::Alliance::Self);
    for (const auto& unit : units) {
        for (const auto& order : unit->orders) {
            if (order.ability_id == ability_type_for_structure) {
                return false;
            }
        }
    }

    // Just try a random location near the unit.
    const Unit* unit = nullptr;
    if (!GetRandomUnit(unit, observation, unit_type))
        return false;

    float rx = GetRandomScalar();
    float ry = GetRandomScalar();

    Actions()->UnitCommand(unit, ability_type_for_structure, unit->pos + Point2D(rx, ry) * 15.0f);
    return true;
}

bool TerranBot::TryBuildSupplyDepot() {
    const ObservationInterface* observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
        return false;

    // Try and build a depot. Find a random TERRAN_SCV and give it the order.
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
}

bool TerranBot::TryBuildBarracks() {
    const ObservationInterface* observation = Observation();

    // Wait until we have our quota of TERRAN_SCV's.
    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_SCV) < TargetSCVCount)
        return false;

    // One build 1 barracks.
    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_BARRACKS) > 0)
        return false;

    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

bool TerranBot::TryBuildUnit(AbilityID ability_type_for_unit, UnitTypeID unit_type) {
    const ObservationInterface* observation = Observation();

    const Unit* unit = nullptr;
    if (!GetRandomUnit(unit, observation, unit_type))
        return false;

    if (!unit->orders.empty())
        return false;

    Actions()->UnitCommand(unit, ability_type_for_unit);
    return true;
}

bool TerranBot::TryBuildSCV() {
    if (CountUnitType(Observation(), UNIT_TYPEID::TERRAN_SCV) >= TargetSCVCount)
        return false;

    return TryBuildUnit(ABILITY_ID::TRAIN_SCV, UNIT_TYPEID::TERRAN_COMMANDCENTER);
}

bool TerranBot::TryBuildMarine() {
    return TryBuildUnit(ABILITY_ID::TRAIN_MARINE, UNIT_TYPEID::TERRAN_BARRACKS);
}

void TerranBot::OnStep() {
    // If there are marines and the command center is not found, send them scouting.
    ScoutWithMarines();

    // Build supply depots if they are needed.
    if (TryBuildSupplyDepot())
        return;

    // Build TERRAN_SCV's if they are needed.
    if (TryBuildSCV())
        return;

    // Build Barracks if they are ready to be built.
    if (TryBuildBarracks())
        return;

    // Just keep building marines if possible.
    if (TryBuildMarine())
        return;
}

void TerranBot::OnGameEnd() {
    std::cout << "Game Ended for: " << std::to_string(Control()->Proto().GetAssignedPort()) << std::endl;
}
*/
void MarineMicroBot::OnGameStart() {
    move_back_ = false;
    targeted_zergling_ = 0;
}

void MarineMicroBot::OnStep() {
    const ObservationInterface* observation = Observation();
    ActionInterface* action = Actions();

    Point2D mp, zp;

    if (!GetPosition(UNIT_TYPEID::TERRAN_MARINE, Unit::Alliance::Self, mp)) {
        return;
    }

    if (!GetPosition(UNIT_TYPEID::ZERG_ZERGLING, Unit::Alliance::Enemy, zp)) {
        return;
    }

    if (!GetNearestZergling(mp)) {
        return;
    }

    Units units = observation->GetUnits(Unit::Alliance::Self);
    for (const auto& u : units) {
        switch (static_cast<UNIT_TYPEID>(u->unit_type)) {
        case UNIT_TYPEID::TERRAN_MARINE: {
            if (!move_back_) {
                action->UnitCommand(u, ABILITY_ID::ATTACK, targeted_zergling_);
            }
            else {
                if (Distance2D(mp, backup_target_) < 1.5f) {
                    move_back_ = false;
                }

                action->UnitCommand(u, ABILITY_ID::SMART, backup_target_);
            }
            break;
        }
        default: {
            break;
        }
        }
    }
}

void MarineMicroBot::OnUnitDestroyed(const Unit* unit) {
    if (unit == targeted_zergling_) {
        Point2D mp, zp;
        if (!GetPosition(UNIT_TYPEID::TERRAN_MARINE, Unit::Alliance::Self, mp)) {
            return;
        }

        if (!GetPosition(UNIT_TYPEID::ZERG_ZERGLING, Unit::Alliance::Enemy, zp)) {
            return;
        }

        Vector2D diff = mp - zp;
        Normalize2D(diff);

        targeted_zergling_ = 0;
        move_back_ = true;
        backup_start_ = mp;
        backup_target_ = mp + diff * 3.0f;
    }
}

bool MarineMicroBot::GetPosition(UNIT_TYPEID unit_type, Unit::Alliance alliace, Point2D& position) {
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(alliace);

    if (units.empty()) {
        return false;
    }

    position = Point2D(0.0f, 0.0f);
    unsigned int count = 0;

    for (const auto& u : units) {
        if (u->unit_type == unit_type) {
            position += u->pos;
            ++count;
        }
    }

    position /= (float)count;

    return true;
}

bool MarineMicroBot::GetNearestZergling(const Point2D& from) {
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Alliance::Enemy);

    if (units.empty()) {
        return false;
    }

    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        if (u->unit_type == UNIT_TYPEID::ZERG_ZERGLING) {
            float d = DistanceSquared2D(u->pos, from);
            if (d < distance) {
                distance = d;
                targeted_zergling_ = u;
            }
        }
    }

    return true;
}












}
