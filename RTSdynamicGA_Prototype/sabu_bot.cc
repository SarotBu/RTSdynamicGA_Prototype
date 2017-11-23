#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"

#include "bot_examples.h"
#include "sabulib.h"
#include <iostream>

int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    coordinator.SetMultithreaded(true);
    sc2::SaBuBot sabuBot;
    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &sabuBot),
        CreateComputer(sc2::Race::Terran,sc2::Difficulty::Hard)
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    bool do_break = false;

    for (; !do_break;) {
        coordinator.StartGame(sc2::kMapBelShirVestigeLE);
        while (coordinator.Update() && !do_break) {
            if (sc2::PollKeyPress()) {
                do_break = true;
            }
        }
    }
    sabuBot.Control()->DumpProtoUsage();
    return 0;
}
