#include "sabulib.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <iterator>

#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"

using namespace sc2;

bool SabuBot::executeScriptViaID(int scriptID) {
	if (scriptID == 1) {
		return ruleFunc1();
	}
	else	if (scriptID == 2) {
		return ruleFunc2();
	}
	else	if (scriptID == 3) {
		return ruleFunc3();
	}
	else	if (scriptID == 4) {
		return ruleFunc4();
	}
	else	if (scriptID == 5) {
		return ruleFunc5();
	}
	else	if (scriptID == 6) {
		return ruleFunc6();
	}
	else	if (scriptID == 7) {
		return ruleFunc7();
	}
	else	if (scriptID == 8) {
		return ruleFunc8();
	}
	else	if (scriptID == 9) {
		return ruleFunc9();
	}
	else	if (scriptID == 10) {
		return ruleFunc10();
	}
	else	if (scriptID == 11) {
		return ruleFunc11();
	}
	else	if (scriptID == 12) {
		return ruleFunc12();
	}
	else	if (scriptID == 13) {
		return ruleFunc13();
	}
	else	if (scriptID == 14) {
		return ruleFunc14();
	}
	else	if (scriptID == 15) {
		return ruleFunc15();
	}
	else	if (scriptID == 16) {
		return ruleFunc16();
	}
	else	if (scriptID == 17) {
		return ruleFunc17();
	}
	else	if (scriptID == 18) {
		return ruleFunc18();
	}
	else	if (scriptID == 19) {
		return ruleFunc19();
	}
	else	if (scriptID == 20) {
		return ruleFunc20();
	}
	else	if (scriptID == 21) {
		return ruleFunc21();
	}
	else	if (scriptID == 22) {
		return ruleFunc22();
	}
	else	if (scriptID == 23) {
		return ruleFunc23();
	}
	else	if (scriptID == 24) {
		return ruleFunc24();
	}
	else	if (scriptID == 25) {
		return ruleFunc25();
	}
	else	if (scriptID == 26) {
		return ruleFunc26();
	}
	else	if (scriptID == 27) {
		return ruleFunc27();
	}
	else	if (scriptID == 28) {
		return ruleFunc28();
	}
	else	if (scriptID == 29) {
		return ruleFunc29();
	}
	else	if (scriptID == 30) {
		return ruleFunc30();
	}
	else	if (scriptID == 31) {
		return ruleFunc31();
	}
	else	if (scriptID == 32) {
		return ruleFunc32();
	}
	else	if (scriptID == 33) {
		return ruleFunc33();
	}
	else	if (scriptID == 34) {
		return ruleFunc34();
	}
	else	if (scriptID == 35) {
		return ruleFunc35();
	}
	else	if (scriptID == 36) {
		return ruleFunc36();
	}
	else	if (scriptID == 37) {
		return ruleFunc37();
	}
	else	if (scriptID == 38) {
		return ruleFunc38();
	}
	else	if (scriptID == 39) {
		return ruleFunc39();
	}
	else	if (scriptID == 40) {
		return ruleFunc40();
	}
	else	if (scriptID == 41) {
		return ruleFunc41();
	}
	else	if (scriptID == 42) {
		return ruleFunc42();
	}
	else	if (scriptID == 43) {
		return ruleFunc43();
	}
	else	if (scriptID == 44) {
		return ruleFunc44();
	}
	else	if (scriptID == 45) {
		return ruleFunc45();
	}
	else	if (scriptID == 46) {
		return ruleFunc46();
	}
	else	if (scriptID == 47) {
		return ruleFunc47();
	}
	else	if (scriptID == 48) {
		return ruleFunc48();
	}
	else	if (scriptID == 49) {
		return ruleFunc49();
	}
	else	if (scriptID == 50) {
		return ruleFunc50();
	}
	else	if (scriptID == 51) {
		return ruleFunc51();
	}
	else	if (scriptID == 52) {
		return ruleFunc52();
	}
	else	if (scriptID == 53) {
		return ruleFunc53();
	}
	else	if (scriptID == 54) {
		return ruleFunc54();
	}
	else	if (scriptID == 55) {
		return ruleFunc55();
	}
	else	if (scriptID == 56) {
		return ruleFunc56();
	}
	else	if (scriptID == 57) {
		return ruleFunc57();
	}
	else	if (scriptID == 58) {
		return ruleFunc58();
	}
	else	if (scriptID == 59) {
		return ruleFunc59();
	}
	else	if (scriptID == 60) {
		return ruleFunc60();
	}
	else	if (scriptID == 61) {
		return ruleFunc61();
	}
	else	if (scriptID == 62) {
		return ruleFunc62();
	}
	else	if (scriptID == 63) {
		return ruleFunc63();
	}
	else	if (scriptID == 64) {
		return ruleFunc64();
	}
	else	if (scriptID == 65) {
		return ruleFunc65();
	}
	else	if (scriptID == 66) {
		return ruleFunc66();
	}
	else	if (scriptID == 67) {
		return ruleFunc67();
	}
	else	if (scriptID == 68) {
		return ruleFunc68();
	}
	else	if (scriptID == 69) {
		return ruleFunc69();
	}
	else	if (scriptID == 70) {
		return ruleFunc70();
	}
	else	if (scriptID == 71) {
		return ruleFunc71();
	}
	else	if (scriptID == 72) {
		return ruleFunc72();
	}
	else	if (scriptID == 73) {
		return ruleFunc73();
	}
	else	if (scriptID == 74) {
		return ruleFunc74();
	}
	else	if (scriptID == 75) {
		return ruleFunc75();
	}
	else	if (scriptID == 76) {
		return ruleFunc76();
	}
	else	if (scriptID == 77) {
		return ruleFunc77();
	}
	else	if (scriptID == 78) {
		return ruleFunc78();
	}
	else	if (scriptID == 79) {
		return ruleFunc79();
	}
	else	if (scriptID == 80) {
		return ruleFunc80();
	}
	else	if (scriptID == 81) {
		return ruleFunc81();
	}
	else	if (scriptID == 82) {
		return ruleFunc82();
	}
	else	if (scriptID == 83) {
		return ruleFunc83();
	}
	else	if (scriptID == 84) {
		return ruleFunc84();
	}
	else	if (scriptID == 85) {
		return ruleFunc85();
	}
	else	if (scriptID == 86) {
		return ruleFunc86();
	}
	else	if (scriptID == 87) {
		return ruleFunc87();
	}
	else	if (scriptID == 88) {
		return ruleFunc88();
	}
	else	if (scriptID == 89) {
		return ruleFunc89();
	}
	else	if (scriptID == 90) {
		return ruleFunc90();
	}
	else	if (scriptID == 91) {
		return ruleFunc91();
	}
	else	if (scriptID == 92) {
		return ruleFunc92();
	}
	else	if (scriptID == 93) {
		return ruleFunc93();
	}
	else	if (scriptID == 94) {
		return ruleFunc94();
	}
	else	if (scriptID == 95) {
		return ruleFunc95();
	}
	else	if (scriptID == 96) {
		return ruleFunc96();
	}
	else	if (scriptID == 97) {
		return ruleFunc97();
	}
	else	if (scriptID == 98) {
		return ruleFunc98();
	}
	else	if (scriptID == 99) {
		return ruleFunc99();
	}
	else	if (scriptID == 100) {
		return ruleFunc100();
	}
	return false;
}

bool SabuBot::ruleFunc1() {
	return SabuBot::buildORBITALCOMMAND();
}

bool SabuBot::ruleFunc2() {
	//std::cout << "BARRACK GO !\n";
	return SabuBot::buildBARRACK();
}

bool SabuBot::ruleFunc3() {
	return SabuBot::buildFACTORY();
}

bool SabuBot::ruleFunc4() {
	//std::cout << "WHAT STARPORT !?\n";
	return SabuBot::buildSTARPORT();
}

bool SabuBot::ruleFunc5() {
	return SabuBot::researchTERRANINFANTRYWEAPONSLEVEL1();
}

bool SabuBot::ruleFunc6() {
	return SabuBot::researchTERRANINFANTRYWEAPONSLEVEL2();
}

bool SabuBot::ruleFunc7() {
	return SabuBot::researchTERRANINFANTRYWEAPONSLEVEL3();
}

bool SabuBot::ruleFunc8() {
	return SabuBot::researchCONCUSSIVESHELLS();
}

bool SabuBot::ruleFunc9() {
	return SabuBot::researchCOMBATSHIELD();
}

bool SabuBot::ruleFunc10() {
	return SabuBot::researchSTIMPACK();
}

bool SabuBot::ruleFunc11() {
	return SabuBot::researchTERRANVEHICLEWEAPONSLEVEL1();
}

bool SabuBot::ruleFunc12() {
	return SabuBot::researchTERRANVEHICLEWEAPONSLEVEL2();
}

bool SabuBot::ruleFunc13() {
	return SabuBot::researchTERRANVEHICLEWEAPONSLEVEL3();
}

bool SabuBot::ruleFunc14() {
	return SabuBot::buildMEDIVAC(1);
}

bool SabuBot::ruleFunc15() {
	return SabuBot::buildMEDIVAC(2);
}

bool SabuBot::ruleFunc16() {
	return SabuBot::buildMARINE(1);
}

bool SabuBot::ruleFunc17() {
	return SabuBot::buildMARINE(2);
}

bool SabuBot::ruleFunc18() {
	return SabuBot::buildMARAUDER(1);
}

bool SabuBot::ruleFunc19() {
	return SabuBot::buildMARAUDER(2);
}

bool SabuBot::ruleFunc20() {
	return SabuBot::TryBuildExpansionCom();
}

bool SabuBot::ruleFunc21() {
	return SabuBot::buildFACTORYTECHLAB();
}

bool SabuBot::ruleFunc22() {
	return SabuBot::buildBARRACKTECHLAB();
}

bool SabuBot::ruleFunc23() {
	return SabuBot::buildVIKING(1);
}

bool SabuBot::ruleFunc24() {
	return SabuBot::buildVIKING(2);
}

bool SabuBot::ruleFunc25() {
	return SabuBot::buildMEDIVAC(4);
}

bool SabuBot::ruleFunc26() {
	return SabuBot::buildBARRACKREACTOR();
}

bool SabuBot::ruleFunc27() {
	return SabuBot::buildFACTORYREACTOR();
}

bool SabuBot::ruleFunc28() {
	return SabuBot::buildSTARPORTTECHLAB();
}

bool SabuBot::ruleFunc29() {
	return SabuBot::buildSTARPORTREACTOR();
}

bool SabuBot::ruleFunc30() {
	return SabuBot::BuildRefinery();
}

bool SabuBot::ruleFunc31() {
	bool chk1, chk2;
	chk1 = chk2 = false;
	chk1 = SabuBot::BuildRefinery();
	chk2 = SabuBot::BuildRefinery();
	return chk1&chk2;
}

bool SabuBot::ruleFunc32() {
	bool chk1, chk2;
	chk1 = chk2 = false;
	chk1 = SabuBot::TryBuildExpansionCom();
	chk2 = SabuBot::TryBuildExpansionCom();
	return chk1&chk2;
}

bool SabuBot::ruleFunc33() {
	return SabuBot::buildBANSHEE(1);
}

bool SabuBot::ruleFunc34() {
	return SabuBot::buildBANSHEE(2);
}

bool SabuBot::ruleFunc35() {
	return SabuBot::buildHELLION(1);
}

bool SabuBot::ruleFunc36() {
	return SabuBot::buildHELLION(2);
}

bool SabuBot::ruleFunc37() {
	return SabuBot::buildSIEGETANK(1);
}

bool SabuBot::ruleFunc38() {
	return SabuBot::buildSIEGETANK(2);
}

bool SabuBot::ruleFunc39() {
	return SabuBot::researchTERRANSHIPWEAPONSLEVEL1();
}

bool SabuBot::ruleFunc40() {
	return SabuBot::researchTERRANSHIPWEAPONSLEVEL2();
}

bool SabuBot::ruleFunc41() {
	return SabuBot::researchTERRANSHIPWEAPONSLEVEL3();
}

bool SabuBot::ruleFunc42() {
	return SabuBot::researchTERRANINFANTRYARMORLEVEL1();
}

bool SabuBot::ruleFunc43() {
	return SabuBot::researchTERRANINFANTRYARMORLEVEL2();
}

bool SabuBot::ruleFunc44() {
	return SabuBot::researchTERRANINFANTRYARMORLEVEL3();
}

bool SabuBot::ruleFunc45() {
	return SabuBot::buildARMORY();
}

bool SabuBot::ruleFunc46() {
	return SabuBot::buildGHOSTACADEMY();
}

bool SabuBot::ruleFunc47() {
	return SabuBot::buildFUSIONCORE();
}

bool SabuBot::ruleFunc48() {
	return SabuBot::buildTHOR(1);
}

bool SabuBot::ruleFunc49() {
	return SabuBot::buildTHOR(2);
}

bool SabuBot::ruleFunc50() {
	return SabuBot::buildMARINE(3);
}

bool SabuBot::ruleFunc51() {
	return SabuBot::buildMARINE(4);
}

bool SabuBot::ruleFunc52() {
	return SabuBot::buildMARINE(5);
}

bool SabuBot::ruleFunc53() {
	return SabuBot::buildMARAUDER(3);
}

bool SabuBot::ruleFunc54() {
	return SabuBot::buildMARAUDER(4);
}

bool SabuBot::ruleFunc55() {
	return SabuBot::buildMARAUDER(5);
}

bool SabuBot::ruleFunc56() {
	return SabuBot::buildMEDIVAC(3);
}

bool SabuBot::ruleFunc57() {
	return SabuBot::buildMEDIVAC(4);
}

bool SabuBot::ruleFunc58() {
	return SabuBot::buildMEDIVAC(5);
}

bool SabuBot::ruleFunc59() {
	return SabuBot::buildSIEGETANK(3);
}

bool SabuBot::ruleFunc60() {
	return SabuBot::buildSIEGETANK(4);
}

bool SabuBot::ruleFunc61() {
	return SabuBot::buildSIEGETANK(5);
}

bool SabuBot::ruleFunc62() {
	return SabuBot::buildHELLION(3);
}

bool SabuBot::ruleFunc63() {
	return SabuBot::buildHELLION(4);
}

bool SabuBot::ruleFunc64() {
	return SabuBot::buildHELLION(5);
}

bool SabuBot::ruleFunc65() {
	bool chk1, chk2;
	chk1 = chk2 = false;
	chk1 = SabuBot::buildBARRACK();
	chk2 = SabuBot::buildBARRACK();
	return chk1&chk2;
}

bool SabuBot::ruleFunc66() {
	bool chk1, chk2;
	chk1 = chk2 = false;
	chk1 = SabuBot::buildFACTORY();
	chk2 = SabuBot::buildFACTORY();
	return chk1&chk2;
}

bool SabuBot::ruleFunc67() {
	bool chk1, chk2;
	chk1 = chk2 = false;
	chk1 = SabuBot::buildSTARPORT();
	chk2 = SabuBot::buildSTARPORT();
	return chk1&chk2;
}

bool SabuBot::ruleFunc68() {
	return false;
	bool chk1, chk2;
	chk1 = chk2 = false;
	chk1 = SabuBot::buildSTARPORTREACTOR();
	chk2 = SabuBot::buildSTARPORTREACTOR();
	return chk1&chk2;
}

bool SabuBot::ruleFunc69() {
	return false;
	bool chk1, chk2;
	chk1 = chk2 = false;
	chk1 = SabuBot::buildFACTORYREACTOR();
	chk2 = SabuBot::buildFACTORYREACTOR();
	return chk1&chk2;
}

bool SabuBot::ruleFunc70() {
	return false;
	bool chk1, chk2;
	chk1 = chk2 = false;
	chk1 = SabuBot::buildBARRACKREACTOR();
	chk2 = SabuBot::buildBARRACKREACTOR();
	return chk1&chk2;
}

bool SabuBot::ruleFunc71() {
	return false;
}

bool SabuBot::ruleFunc72() {
	return false;
}

bool SabuBot::ruleFunc73() {
	return false;
}

bool SabuBot::ruleFunc74() {
	return false;
}

bool SabuBot::ruleFunc75() {
	return false;
}

bool SabuBot::ruleFunc76() {
	return false;
}

bool SabuBot::ruleFunc77() {
	return false;
}

bool SabuBot::ruleFunc78() {
	return false;
}

bool SabuBot::ruleFunc79() {
	return false;
}

bool SabuBot::ruleFunc80() {
	return false;
}

bool SabuBot::ruleFunc81() {
	return false;
}

bool SabuBot::ruleFunc82() {
	return false;
}

bool SabuBot::ruleFunc83() {
	return false;
}

bool SabuBot::ruleFunc84() {
	return false;
}

bool SabuBot::ruleFunc85() {
	return false;
}

bool SabuBot::ruleFunc86() {
	return false;
}

bool SabuBot::ruleFunc87() {
	return false;
}

bool SabuBot::ruleFunc88() {
	return false;
}

bool SabuBot::ruleFunc89() {
	return false;
}

bool SabuBot::ruleFunc90() {
	return false;
}

bool SabuBot::ruleFunc91() {
	return false;
}

bool SabuBot::ruleFunc92() {
	return false;
}

bool SabuBot::ruleFunc93() {
	return false;
}

bool SabuBot::ruleFunc94() {
	return false;
}

bool SabuBot::ruleFunc95() {
	return false;
}

bool SabuBot::ruleFunc96() {
	return false;
}

bool SabuBot::ruleFunc97() {
	return false;
}

bool SabuBot::ruleFunc98() {
	return false;
}

bool SabuBot::ruleFunc99() {
	return false;
}

bool SabuBot::ruleFunc100() {
	return false;
}


//void sc2::SabuBot::AgentTakeAction() {
//	//	getCurrentState
//}
//	
//	void SabuBot::ruleFunc1();
//	
//	void SabuBot::ruleFunc2();
//	void SabuBot::ruleFunc3();
//	void SabuBot::ruleFunc4();
//	void SabuBot::ruleFunc5();
//	void SabuBot::ruleFunc6();
//	void SabuBot::ruleFunc7();
//	void SabuBot::ruleFunc8();
//	void SabuBot::ruleFunc9();
//	void SabuBot::ruleFunc10();
//	void SabuBot::ruleFunc11();
//	void SabuBot::ruleFunc12();
//	void SabuBot::ruleFunc13();
//	void SabuBot::ruleFunc14();
//	void SabuBot::ruleFunc15();
//	void SabuBot::ruleFunc16();
//	void SabuBot::ruleFunc17();
//	void SabuBot::ruleFunc18();
//	void SabuBot::ruleFunc19();
//	void SabuBot::ruleFunc20();
//	void SabuBot::ruleFunc21();
//	void SabuBot::ruleFunc22();
//	void SabuBot::ruleFunc23();
//	void SabuBot::ruleFunc24();
//	void SabuBot::ruleFunc25();
//	*/
//	void executeScriptViaID(int scriptID){
//		if (scriptID == 1) {
//			SabuBot::ruleFunc1();
//		}
//
//		/*else	if (scriptID == 2) {
//			SabuBot::ruleFunc2();
//		}
//		else	if (scriptID == 3) {
//			SabuBot::ruleFunc3();
//		}
//		else	if (scriptID == 4) {
//			SabuBot::ruleFunc4();
//		}
//		else	if (scriptID == 5) {
//			SabuBot::ruleFunc5();
//		}
//		else	if (scriptID == 6) {
//			SabuBot::ruleFunc6();
//		}
//		else	if (scriptID == 7) {
//			SabuBot::ruleFunc7();
//		}
//		else	if (scriptID == 8) {
//			SabuBot::ruleFunc8();
//		}
//		else	if (scriptID == 9) {
//			SabuBot::ruleFunc9();
//		}
//		else	if (scriptID == 10) {
//			SabuBot::ruleFunc10();
//		}
//		else	if (scriptID == 11) {
//			SabuBot::ruleFunc11();
//		}
//		else	if (scriptID == 12) {
//			SabuBot::ruleFunc12();
//		}
//		else	if (scriptID == 13) {
//			SabuBot::ruleFunc13();
//		}
//		else	if (scriptID == 14) {
//			SabuBot::ruleFunc14();
//		}
//		else	if (scriptID == 15) {
//			SabuBot::ruleFunc15();
//		}
//		else	if (scriptID == 16) {
//			SabuBot::ruleFunc16();
//		}
//		else	if (scriptID == 17) {
//			SabuBot::ruleFunc17();
//		}
//		else	if (scriptID == 18) {
//			SabuBot::ruleFunc18();
//		}
//		else	if (scriptID == 19) {
//			SabuBot::ruleFunc19();
//		}
//		else	if (scriptID == 20) {
//			SabuBot::ruleFunc20();
//		}
//		else	if (scriptID == 21) {
//			SabuBot::ruleFunc21();
//		}
//		else	if (scriptID == 22) {
//			SabuBot::ruleFunc22();
//		}
//		else	if (scriptID == 23) {
//			SabuBot::ruleFunc23();
//		}
//		else	if (scriptID == 24) {
//			SabuBot::ruleFunc24();
//		}
//		else	if (scriptID == 25) {
//			SabuBot::ruleFunc25();
//		}*/
//	}
//
//	void SabuBot::ruleFunc1() {
//		//SabuBot::buildORBITALCOMMAND();
//		//buildORBITALCOMMAND();
//		//sc2::SabuBot
//	}
//
//	/*
//	void SabuBot::ruleFunc2() {
//		SabuBot::buildBARRACKADDON();
//	}
//
//	void SabuBot::ruleFunc3() {
//		SabuBot::buildFACTORY();
//	}
//
//	void SabuBot::ruleFunc4() {
//		SabuBot::buildSTARPORT();
//	}
//
//	void SabuBot::ruleFunc5() {
//		SabuBot::researchTERRANINFANTRYWEAPONSLEVEL1();
//	}
//
//	void SabuBot::ruleFunc6() {
//		SabuBot::researchTERRANINFANTRYWEAPONSLEVEL2();
//	}
//
//	void SabuBot::ruleFunc7() {
//		SabuBot::researchTERRANINFANTRYWEAPONSLEVEL3();
//	}
//
//	void SabuBot::ruleFunc8() {
//		SabuBot::researchCONCUSSIVESHELLS();
//	}
//
//	void SabuBot::ruleFunc9() {
//		SabuBot::researchCOMBATSHIELD();
//	}
//
//	void SabuBot::ruleFunc10() {
//		SabuBot::researchSTIMPACK();
//	}
//
//	void SabuBot::ruleFunc11() {
//		SabuBot::researchTERRANVEHICLEWEAPONSLEVEL1();
//	}
//
//	void SabuBot::ruleFunc12() {
//		SabuBot::researchTERRANVEHICLEWEAPONSLEVEL2();
//	}
//
//	void SabuBot::ruleFunc13() {
//		SabuBot::researchTERRANVEHICLEWEAPONSLEVEL3();
//	}
//
//	void SabuBot::ruleFunc14() {
//		SabuBot::buildVIKING(5);
//	}
//
//	void SabuBot::ruleFunc15() {
//		SabuBot::buildMEDIVAC(5);
//	}
//
//	void SabuBot::ruleFunc16() {
//		SabuBot::buildMARINE(5);
//	}
//
//	void SabuBot::ruleFunc17() {
//		SabuBot::buildMARAUDER(5);
//	}
//
//	void SabuBot::ruleFunc18() {
//		SabuBot::buildMARINE(6);
//	}
//
//	void SabuBot::ruleFunc19() {
//		SabuBot::buildMARINE(7);
//	}
//
//	void SabuBot::ruleFunc20() {
//		SabuBot::buildMARINE(8);
//	}
//
//	void SabuBot::ruleFunc21() {
//		SabuBot::buildMARINE(9);
//	}
//
//	void SabuBot::ruleFunc22() {
//		SabuBot::buildMARINE(10);
//	}
//
//	void SabuBot::ruleFunc23() {
//		SabuBot::buildMEDIVAC(2);
//	}
//
//	void SabuBot::ruleFunc24() {
//		SabuBot::buildMEDIVAC(3);
//	}
//
//	void SabuBot::ruleFunc25() {
//		SabuBot::buildMEDIVAC(4);
//	}
//	*/
//	