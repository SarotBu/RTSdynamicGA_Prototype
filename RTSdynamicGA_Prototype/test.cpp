#include <cstdio>
#include <random>

//std::random_device gen;
//std::uniform_int_distribution<int> dist(0, 5);

void test(){
    std::random_device gen;
    std::uniform_int_distribution<int> dist(0, 5);
    printf ("1 : %d\n",dist(gen));
    printf ("1 : %d\n",dist(gen));
    printf ("1 : %d\n",dist(gen));
    printf ("1 : %d\n",dist(gen));
    printf ("1 : %d\n",dist(gen));
}

void test2(){
    std::random_device gen;
    std::uniform_int_distribution<int> dist(0, 5);
    printf ("2 : %d\n",dist(gen));
    printf ("2 : %d\n",dist(gen));
    printf ("2 : %d\n",dist(gen));
    printf ("2 : %d\n",dist(gen));
    printf ("2 : %d\n",dist(gen));
}

int main() {
//initialize();
    test();
    test2();
return 0;
}
