#include <cstdio>
#include <random>
#include <iostream>

std::mt19937 gen;
std::mt19937 gen2;
std::default_random_engine eng(gen());
std::uniform_int_distribution<int> dist(0, 5);

void test(){
    //std::random_device gen;
    std::uniform_int_distribution<int> dist(0, 5);
    printf ("1 : %d\n",dist(gen));
    printf ("1 : %d\n",dist(gen));
    printf ("1 : %d\n",dist(gen));
    printf ("1 : %d\n",dist(gen));
    printf ("1 : %d\n",dist(gen));
}

void test2(){
    //std::random_device gen;
    std::uniform_int_distribution<int> dist(0, 5);
    printf ("2 : %d\n",dist(gen));
    printf ("2 : %d\n",dist(gen));
    printf ("2 : %d\n",dist(gen));
    printf ("2 : %d\n",dist(gen));
    printf ("2 : %d\n",dist(gen));
}

int main() {
//initialize();
    //gen.seed_seq(std::random_device());
//    std::cout << gen.entropy() << std::endl;
    //gen.seed(std::mt19937_64::default_seed);
    test();
    test2();
return 0;
}
