//#include <bits/stdc++.h>
#include <vector>
#include <random>
#include "sabu_rand.h"
//#include "../../GA_SC2.cpp"

namespace sc2 {
    namespace sb {
#define RULENUMS 100

#define LOOPS_LIMIT -1 // USED TO LIMIT NUMBERS OF LOOP RUNNING GA, -1 = INFINITY, INT = # of LOOPS limit
#define GENE_LENGTH 81 // LENGTH OF STRING OF SUDOKU
#define MINVAL 1 //MIN VAL FOR SUDOKU
#define MAXVAL 9 //MAX VAL FOR SUDOKU
#define POPULATION_SIZE 100
#define MUTATION_RATE 0.1
#define CROSSOVER_RATE 0.1
#define REMAINING_INDIVIDUALS_SIZE 30
#define CROSSOVER_INDIVIDUALS_SIZE 20
#define MUTATED_INDIVIDUALS_SIZE 30
#define FILLED_INDIVIDUALS_SIZE 20
#define LOCAL_STRUCK_TIMES 10000
#define OLD_GEN_AFTER_RE_NUMS 0
#define MAX_STATE_TIER 10
#define INITIAL_STATE 1 // INITIAL STATE FOR RTS AI

#define MIN_ECOVAL 0
#define MAX_ECOVAL 5
#define MIN_COMVAL 0
#define MAX_COMVAL 5

#define BuildScriptN 10
#define ResearchScriptN 10
#define EconomyScriptN 10
#define CombatScriptN 10

        typedef long long ll;
        typedef unsigned int uint;
        typedef unsigned long long ull;


        class Rule {
        private:
            int scriptID, stateID;
            bool used = 0;
        protected:
            //virtual void randomize();
            //std::random_device rndgen;
            //virtual void randomize() override;
        public:
            int getScriptID();
            int getStateID();
            bool getUsed();
            void setScriptID(int sid);
            void setStateID(int sid);
            void setUsed(bool _used);
        };

        class BuildRule : public Rule {
        };

        class ResearchRule : public Rule {
        };

        class EconomyRule : public Rule {
            int param;
            int getParam();
            void setParam(int sval);
        };

        class CombatRule : public Rule {
            int param;
            int getParam();
            void setParam(int sval);
        };

        class StateRule : public Rule {
        };


        class Script {
            std::vector <Rule> ruleList;
        };


        //std::map <int, std::vector<Script> > scriptList; // scriptList[State] -> Script1 -> Script2 -> ...
        /* RULES STATE for TERRAN

        */
    }
}