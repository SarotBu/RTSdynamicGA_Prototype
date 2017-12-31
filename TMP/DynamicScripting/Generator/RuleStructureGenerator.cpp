//RuleStructureGenerator.cpp
#include <cstdio>
int N;

void genLibs(){
    printf ("#include \"RuleData.cpp\"\n");
}

void writeHeader(int ruleID){
    printf ("bool ruleFunc%d();\n",ruleID);
}

void writeRule(int ruleID){
    printf ("\t\treturn ruleFunc%d();\n",ruleID);
}

void writeFuncBody(int ruleID){
    printf ("bool SabuBot::ruleFunc%d(){\n",ruleID);
    printf ("\t\treturn SabuBot::");
    printf (" \n}\n\n");
}

void genHeader(){
    for (int i=1;i<=N;i++){
        writeHeader(i);
    }
}

void genRules(){
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
    printf ("bool SabuBot::executeScriptViaID(int scriptID){\n");
    genRules();
    printf ("}\n\n");
}

void genFuncBody(){
    for (int i=1;i<=N;i++){
        writeFuncBody(i);
    }
}

int main(){
    char fname[50];
    printf ("Function Manager's File name : ");
    scanf ("%s",fname);
    printf ("Please input the number of rules : ");
    freopen(fname,"w",stdout);
    scanf ("%d",&N);
    genLibs();
    genHeader();
    genFunc();
    genFuncBody();
return 0;
}
