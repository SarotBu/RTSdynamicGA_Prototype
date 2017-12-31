#include "sabu_bot.h"
#include "bot_examples.h" // DEFINED ACTIONS HERE
#include "Ruleset.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <iterator>

#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"

namespace sc2 {
	void SabuBot::AgentTakeAction() {
		if (Observation()->GetGameLoop() % 200) return;
		uint64_t curState = calculateCurrentState();
		uint64_t myState = getState();
		std::string myText = "";
		if (myState != curState) {
			std::string sState = std::to_string(curState);
			std::string myText = "Current State : " + sState;
			Actions()->SendChat(myText);
			long double currentScore = calculateCurrentScore();
			std::string myScoreTxt = std::to_string(currentScore);
			myText = "Score : " + myScoreTxt;
			Actions()->SendChat(myText);

			updateState(curState);
		}
		
		
		std::random_device rndgen;
		std::uniform_int_distribution<int> pickDist(1, 20);// 5/100
		std::uniform_int_distribution<int> dist(0, RULENUMS-1);
		bool isDone = false;
		int failedCnt = 0;
		while (!isDone) {
			if (pickDist(rndgen) == 1) {
				int tar = dist(rndgen), cnt = 0;
				//std::cout << "tar : " << tar << "\n";
				while (ruleList[tar].isUsed()) {
					//std::cout << "tar : " << tar << "\n";
					tar = dist(rndgen);
					cnt++;
					if (cnt > RULENUMS) return;
				}
				//std::cout << "Rnd Target : " << tar << " ID : " << ruleList[tar].getRuleID() << " with RuleList size " << ruleList.size() << "\n";
				//bool isDone = false;
				isDone = executeScriptViaID(ruleList[tar].getRuleID());
				if (isDone) {
					ruleList[tar].setUsed(true);
					std::cout << "Randomly Completed Action No. " << ruleList[tar].getRuleID() << " !\n";
					//printRuleList();
					std::cout << "Hmmm\n";
				}
			}
			else {
				while (ruleList[targetRule].isUsed() && targetRule < ruleList.size()) {
					targetRule++;
					if (targetRule >= ruleList.size()) break;
				}
				if (targetRule >= ruleList.size()) {
					updateState(curState);
				}
				//std::cout << "Target : " << targetRule << " ID : " << ruleList[targetRule].getRuleID() << " with RuleList size " << ruleList.size() << "\n";
				isDone = executeScriptViaID(ruleList[targetRule].getRuleID());
				//bool isDone = false;
				if (isDone) {
					ruleList[targetRule].setUsed(true);
					std::cout << "Completed Action No. " << ruleList[targetRule].getRuleID() << " !\n";
					targetRule++;
					//printRuleList();
				}
			}
			failedCnt++;
			if (failedCnt > 200) isDone = true;
		}
	}
	

}