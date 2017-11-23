#include <iostream>
#include <cstring>
#include <string>
#include <vector>

std::vector <std::string> encodedBuilding;

void split (const std::string &s, const char* delim, std::vector<std::string> &v){
    char *dup = strdup(s.c_str());
    char *token = strtok(dup, delim);
    while (token != NULL){
        v.push_back(std::string(token));
        token = strtok(NULL, delim);
    }
    free(dup);
}

void showState(){
    for (int i=1;i<encodedBuilding.size();i++)
        std::cout << i << "\t" << encodedBuilding[i]<< std::endl;
}

int main(){
    std::string sline;
    freopen ("StateData.txt","r",stdin);
    while (std::getline(std::cin,sline)){
        std::vector<std::string> v;
        std::vector<std::string> dat;
        split(sline, "|\n", v);
        if (v.size()<2) break;
        encodedBuilding.push_back(v[1]);
    }
    showState();


    return 0;
}
