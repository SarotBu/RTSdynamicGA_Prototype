#include <bits/stdc++.h>
using namespace std;
class Gene{


};

class BuildGene : public Gene{

};

class ResearchGene : public Gene{

};

class EconomyGene : public Gene{

};

class CombatGene : public Gene{

};
int main(){
    CombatGene cgp ;//= new *CombatGene();
    EconomyGene egp ;
    if (typeid(cgp) == typeid(EconomyGene))
    //if (auto cg = dynamic_cast<CombatGene*>(egp))
        cout << "YES" << endl;
    else
        cout << "NOOOOO" << endl;


return 0;
}
