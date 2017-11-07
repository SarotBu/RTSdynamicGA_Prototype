#include <bits/stdc++.h>
#include "Ruleset.h"

#define RULES_LIMIT 100
#define BREAK_EVEN_POINT 0.5 // break-even point is in [0,1]
//vector <int,script> scriptList; // scriptList[State] -> Script1 -> Script2 ->  ...
map<vector<int,script> > scriptList;
int currentGameStateID = 0, currentRuleSelect=0;
long double stateFitness = 0f, overallFitness = 0f;
long double currentPlayerScore, currentOpponentScore, previousStatePlayerScore;

long double calcOverallFitness(){
    long double ret,tmp;
    tmp = currentPlayerScore / (currentPlayerScore + currentOpponentScore);
    if (PlayerWin()){
        return max(tmp,BREAK_EVEN_POINT);
    }else{
        return min(tmp,BREAK_EVEN_POINT);
    }
}

long double calcStateFitness(){
    long double ret;
    ret = currentPlayerScore / (currentPlayerScore + currentOpponentScore);
    if (state != INITIAL_STATE){
        ret = ret - previousStatePlayerScore;
    }
    return ret;
}

void calcScore(){
    currentPlayerScore = 0.7 * playerMilitaryScore + 0.3 * playerBuildingScore;
    currentOpponentScore = 0.7 * opponentMilitaryScore + 0.3 * opponentBuildingScore;
}

void updateRulesProvided(int stateID){

}

void changeState(int newStateID){
    updateRulesProvided(currentStateID);
    currentStateID = newStateID;
    currentRuleSelect = 0;
}

Rule* makeDecision(){
    return &scriptList[currentStateID][currentRuleSelect++];
}

void executeDecision(Rule *selectedRule){
    selectedRule->setUsed(true);
    executeScriptViaID(selectedRule->getScriptID);
}


int main(){
    while (GAME_IS_RUNNING()){
        executeDecision(makeDecision());
        int tmpGameStateID = getCurrentGameStateID();
        if (tmpGameStateID != getCurrentGameStateID()){
            changeState(tmpGameStateID);
        }
    }
    return 0;
}
