//#include <bits/stdc++.h>
#include <vector>
#include <map>
#include <random>
#include "Ruleset.h"
//#include "../../GA_SC2.cpp"

namespace sc2 {
	namespace sb {
		Rule::Rule() {
		
		}
		Rule::Rule(int _ruleID, long double _ruleScore) {
			setRuleID(_ruleID);
			setRuleScore(_ruleScore);
			setUsed(false);
		}

		int Rule::getRuleID() {
			return ruleID;
		}

		long double Rule::getRuleScore() {
			return ruleScore;
		}

		bool Rule::isUsed() {
			return used;
		}

		void Rule::setRuleID(int _ruleID) {
			ruleID = _ruleID;
		}

		void Rule::setRuleScore(long double _val) {
			ruleScore = _val;
		}

		void Rule::setUsed(bool _used) {
			used = _used;
		}

	}
}