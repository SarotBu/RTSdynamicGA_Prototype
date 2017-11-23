#include <iostream>
#include <cstring>
#include <string>
#include <vector>

std::string sline;

void split (const std::string &s, const char* delim, std::vector<std::string> &v){
    char *dup = strdup(s.c_str());
    char *token = strtok(dup, delim);
    while (token != NULL){
        v.push_back(std::string(token));
        token = strtok(NULL, delim);
    }
    free(dup);
}

int main(){
    freopen ("StateData.txt","r",stdin);
    while (std::getline (std::cin,sline)){
//        char *stok = std::strtok(&sline, " ");
    //      std::cout << stok << "\t";
        split()
        std::cout << sline << std::endl;
    }


    return 0;
}
