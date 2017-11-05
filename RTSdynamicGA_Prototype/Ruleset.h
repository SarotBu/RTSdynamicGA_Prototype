#include <bits/stdc++.h>
#include "GA_SC2.cpp"

#define BuildScriptN 10
#define ResearchScriptN 10
#define EconomyScriptN 10
#define CombatScriptN 10

std::random_device gen;

class Rule{
    protected :
    int scriptID, stateID;
    public :
    virtual void randomize();
    int getScriptID(){  return scriptID;    }
    int getStateID(){ return stateID;   }
    void setScriptID(int sid){ scriptID = sid; }
    void setStateID(int sid){stateID = sid; }
};

class BuildRule : public Rule{
    void randomize(){
        std::uniform_int_distribution<int> dist(1,BuildScriptN); // No.0 is a dummy script
        scriptID = dist(gen);
    }
};

class ResearchRule : public Rule{
    void randomize(){
        std::uniform_int_distribution<int> dist(1,ResearchScriptN); // No.0 is a dummy script
        scriptID = dist(gen);
    }
};

class EconomyRule : public Rule{
    int param;
    void randomize(){
        std::uniform_int_distribution<int> dist(1,EconomyScriptN); // No.0 is a dummy script
        std::uniform_int_distribution<int> paramDist(MIN_ECOVAL, MAX_ECOVAL);
        scriptID = dist(gen);
        param = paramDist(gen);
    }
    int getParam(){ return param; }
    void setParam(int sval){ param = sval; }
};

class CombatRule : public Rule{
    int param;
    void randomize(){
        std::uniform_int_distribution<int> dist(1,CombatScriptN); // No.0 is a dummy script
        std::uniform_int_distribution<int> paramDist(MIN_COMVAL, MAX_COMVAL);
        scriptID = dist(gen);
        param = paramDist(gen);
    }
    int getParam(){ return param; }
    void setParam(int sval){ param = sval; }
};

class StateRule : public Rule{

};


class Script{
    vector <Rule> ruleList;
};


vector <int,script> scriptList; // scriptList[State] -> Script1 -> Script2 -> ...
/* RULES STATE for TERRAN

*/


int main(){

    return 0;
}
