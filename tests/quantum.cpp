#include "../drk/quantum.h"
#include <iostream>

int main(){

    drk::linked_quantum<int, int> P{32};
    drk::linked_quantum<int, int> F{8};

    P *= F;

    std::cout << (P != F);

    return 0;
}