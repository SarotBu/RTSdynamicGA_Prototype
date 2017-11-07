//RuleStructureGenerator.cpp
#include <cstdio>

void genLibs(){
    printf ("#include \"RuleData.cpp\"\n");
}

void writeRule(int ruleID){
    printf ("\t\truleFunc%d();\n",ruleID);
}

void genRules(int N){
    //WILL GENERATE FUNCTION ACCORDING TO EACH RULE LATER
    for (int i=1;i<=N;i++){
        printf ("\tif (scriptID == %d){\n",i);
        writeRule(i);
        printf ("\t}");
        if (i<N) printf ("else");
    }
    printf ("\n");
}

void genFunc(){
    printf ("void executeScriptViaID(int scriptID){\n");
    int N;
    scanf ("%d",&N);
    genRules(N);

    printf ("}\n");
}

int main(){
    freopen("RuleManager.cpp","w",stdout);
    genLibs();
    genFunc();
return 0;
}
