#pragma once

#ifdef _MSC_VER

typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

#else
#include <stdint.h>
#endif
namespace sc2 {
    namespace sb {
        class State {
            uint64_t state = 0;

            uint64_t selectBit(int a, int b) {
                uint64_t tmpBase = 1, ret = 0;
                for (int i = a; i <= b; i++) {
                    ret += tmpBase * ((1ll << (i - 1)) & state);
                    tmpBase *= 2;
                }
                return ret;
            }

        public:

            uint64_t getState() {
                return state;
            }

            void setState(uint64_t _state) {
                state = _state;
            }

            bool hasGhostAcademy() {
                return selectBit(1, 1);
            }

            bool hasArmory() {
                return selectBit(2, 2);
            }

            bool hasEngineeringBay() {
                return selectBit(3, 3);
            }

            bool hasCommandCenter() {
                return selectBit(4, 4);
            }

            bool hasOrbitalCommand() {
                return selectBit(5, 5);
            }

            bool hasBarrack() {
                return selectBit(6, 6);
            }

            bool hasBarrackTechlab() {
                return selectBit(7, 7);
            }

            bool hasFactory() {
                return selectBit(8, 8);
            }

            bool hasFactoryTechlab() {
                return selectBit(9, 9);
            }

            bool hasStarport() {
                return selectBit(10, 10);
            }

            bool hasStarportTechlab() {
                return selectBit(11, 11);
            }
        };
    }
}
//STATE DATA FORMAT
/*
ID    |Name
1	|Ghost Academy
2	|Armory
3	|Engineering Bay
4 	|Command Center
5 	|Orbital Command
6	|Barrack
7	|Barrack + Tech Lab
8	|Factory
9	|Factory + Tech Lab
10	|Starport
11	|Starport + Tech Lab
*/