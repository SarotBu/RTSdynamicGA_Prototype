//#include <algorithm>
//#include <map>
//#include <set>
//
////#include "sabulib.h"
//#include "Ruleset.h"
//#include "dynamic_scripting.h"
//
//#include "sc2api/sc2_api.h"
//#include "sc2utils/sc2_manage_process.h"
//
////vector <int,script> scriptList; // scriptList[State] -> Script1 -> Script2 ->  ...
//namespace sc2 {
//	namespace sb {
//			//std::map<sabu::State, std::vector<Script> > scriptList;
//			//std::set<sabu::State> usedStateSet;
//			std::vector<Script> curList;
//			int currentGameState = 0, currentRuleSelect = 0;
//			long double stateFitness = 0.0, overallFitness = 0.0;
//			long double currentPlayerScore, currentOpponentScore, previousStatePlayerScore;
//
//			void initialize() {
//				currentGameState = INITIAL_STATE;
//				//usedStateSet.insert(INITIAL_STATE);
//
//				//loadScript(currentGameStateID);
//			}
//
//			bool PlayerWin() {
//				return true;
//			}
//
//			long double calcOverallFitness() {
//				long double ret, tmp;
//				tmp = currentPlayerScore / (currentPlayerScore + currentOpponentScore);
//				if (PlayerWin()) {
//					return std::max(tmp, BREAK_EVEN_POINT);
//				}
//				else {
//					return std::min(tmp, BREAK_EVEN_POINT);
//				}
//			}
//
//			long double calcStateFitness() {
//				long double ret;
//				ret = currentPlayerScore / (currentPlayerScore + currentOpponentScore);
//				if (currentGameState != INITIAL_STATE) {
//					ret = ret - previousStatePlayerScore;
//				}
//				return ret;
//			}
//
//			void calcScore() {
//
//				//const ObservationInterface* observation = Observation();
//				//currentPlayerScore = 0.7 * playerMilitaryScore + 0.3 * playerBuildingScore;
//				//currentOpponentScore = 0.7 * opponentMilitaryScore + 0.3 * opponentBuildingScore;
//			}
//
//			void updateRulesEmployed(int stateID) {
//				if (overallFitness >= BREAK_EVEN_POINT) {
//
//				}
//				else {
//
//				}
//			}
//
//			//void updateRulesInState(int stateID) {
//			//	scriptList[stateID].clear();
//			//	scriptList[stateID] = curList;
//			//}
//
//			void changeState(int newStateID) {
//				updateRulesEmployed(currentGameState);
//				currentGameState = newStateID;
//				currentRuleSelect = 0;
//				//usedStateSet.insert(newStateID);
//			}
//
//			///*
//			//Rule* makeDecision() {
//			//	return &scriptList[currentGameState][currentRuleSelect++];
//			//}
//			//*/
//
//			//void executeScriptViaID(int scriptID) {
//
//			//}
//
//			//void executeDecision(Rule *selectedRule) {
//			//	selectedRule->setUsed(true);
//			//	//executeScriptViaID(selectedRule->getScriptID);
//			//}
//
//
//
//		/*
//			int main() {
//				initialize();
//				while (true) {
//					loadScript(currentGameStateID);
//					for (int i = 0; i < curList.size(); i++) {
//						executeDecision(&curList[i]);
//						if (curList[i].isStateRule()) {
//							updateRulesEmployed();
//							updateRulesInState(currentGameStateID);
//							changeState(curList[i].targetState);
//
//						}
//					}
//
//
//				}*/
//				/*
//				while (GAME_IS_RUNNING()){
//				executeDecision(makeDecision());
//				int tmpGameStateID = getCurrentGameStateID();
//				if (tmpGameStateID != getCurrentGameStateID()){
//				changeState(tmpGameStateID);
//				}
//				}
//				*/
//				/*
//					return 0;
//				}
//			*/
//		
//	}
//
//}