#pragma once
#include "sc2state.h"
#include "Ruleset.h"
#include "bot_examples.h"

#include <vector>
#include <set>
#include <map>

namespace sc2 {
    class SabuBot : public MainTerranBot {

        sb::State state;
        std::vector<sb::Rule> ruleList;
        std::map<uint64_t,std::set<int> > usedSet;
        uint32_t forceUpdateFrame = 0, timer = 0;
        long double savScore = 0;
        int targetRule = 0;
        

    public:

        //SabuBot();

        void loadRules();

        void saveRules();

        void AgentTakeAction();

        bool executeScriptViaID(int scriptID);

        uint64_t getState();

        void setState(uint64_t _state);

        uint64_t calculateCurrentState();

        long double calculateCurrentScore();

        void updateRuleWeights(uint64_t _state, long double _score);

        void updateEndGameWeights(long double changedScore);

        void updateState(uint64_t _state);

        void forceUpdateState();

        void setForceUpdateFrame(uint32_t _frame);

        void pushRule(int _id, long double _val);

        //sb::Rule getRuleByID(int _id);

        sb::Rule getRulesByIndex(int _idx);

        void printRuleList();

        //void OnStep() override;
        virtual void OnStep();

        virtual void OnGameEnd();

    private:
        //SET IN RuleController.cpp
        bool ruleFunc1();
        bool ruleFunc2();
        bool ruleFunc3();
        bool ruleFunc4();
        bool ruleFunc5();
        bool ruleFunc6();
        bool ruleFunc7();
        bool ruleFunc8();
        bool ruleFunc9();
        bool ruleFunc10();
        bool ruleFunc11();
        bool ruleFunc12();
        bool ruleFunc13();
        bool ruleFunc14();
        bool ruleFunc15();
        bool ruleFunc16();
        bool ruleFunc17();
        bool ruleFunc18();
        bool ruleFunc19();
        bool ruleFunc20();
        bool ruleFunc21();
        bool ruleFunc22();
        bool ruleFunc23();
        bool ruleFunc24();
        bool ruleFunc25();
        bool ruleFunc26();
        bool ruleFunc27();
        bool ruleFunc28();
        bool ruleFunc29();
        bool ruleFunc30();
        bool ruleFunc31();
        bool ruleFunc32();
        bool ruleFunc33();
        bool ruleFunc34();
        bool ruleFunc35();
        bool ruleFunc36();
        bool ruleFunc37();
        bool ruleFunc38();
        bool ruleFunc39();
        bool ruleFunc40();
        bool ruleFunc41();
        bool ruleFunc42();
        bool ruleFunc43();
        bool ruleFunc44();
        bool ruleFunc45();
        bool ruleFunc46();
        bool ruleFunc47();
        bool ruleFunc48();
        bool ruleFunc49();
        bool ruleFunc50();
        bool ruleFunc51();
        bool ruleFunc52();
        bool ruleFunc53();
        bool ruleFunc54();
        bool ruleFunc55();
        bool ruleFunc56();
        bool ruleFunc57();
        bool ruleFunc58();
        bool ruleFunc59();
        bool ruleFunc60();
        bool ruleFunc61();
        bool ruleFunc62();
        bool ruleFunc63();
        bool ruleFunc64();
        bool ruleFunc65();
        bool ruleFunc66();
        bool ruleFunc67();
        bool ruleFunc68();
        bool ruleFunc69();
        bool ruleFunc70();
        bool ruleFunc71();
        bool ruleFunc72();
        bool ruleFunc73();
        bool ruleFunc74();
        bool ruleFunc75();
        bool ruleFunc76();
        bool ruleFunc77();
        bool ruleFunc78();
        bool ruleFunc79();
        bool ruleFunc80();
        bool ruleFunc81();
        bool ruleFunc82();
        bool ruleFunc83();
        bool ruleFunc84();
        bool ruleFunc85();
        bool ruleFunc86();
        bool ruleFunc87();
        bool ruleFunc88();
        bool ruleFunc89();
        bool ruleFunc90();
        bool ruleFunc91();
        bool ruleFunc92();
        bool ruleFunc93();
        bool ruleFunc94();
        bool ruleFunc95();
        bool ruleFunc96();
        bool ruleFunc97();
        bool ruleFunc98();
        bool ruleFunc99();
        bool ruleFunc100();
    };
}