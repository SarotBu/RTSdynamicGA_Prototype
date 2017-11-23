#include <iostream>
#include <cstring>
#include <string>
#include <vector>


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
    std::string sline;
    freopen ("StateData.txt","r",stdin);
    while (std::getline(std::cin,sline)){
        std::cout << "pls\n";
//        char *stok = std::strtok(&sline, " ");
    //      std::cout << stok << "\t";
        std::vector<std::string> v;
        split(sline, " ", v);

        std::cout << v.size() << std::endl;
    }


    return 0;
}
