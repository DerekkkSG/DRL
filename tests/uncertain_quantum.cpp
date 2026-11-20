#include "../drk/quantum.h"

#include <iostream>
#include <cassert>

using namespace std;

#ifndef DEBUG_DRL

// Main testing scene
int main(){

    drk::uncertain_quantum<int> unc_a{{1, 2, 3, 4, 5}};

    std::vector<int> p{1, 2, 3, 4, 5};
    drk::uncertain_quantum<int> unc_b{std::move(p)};

    drk::strict_uncertain<int> unc_c{{1, 2, 3, 4, 5}};

    unc_a.change_values({1, 2, 3});

    return 0;
}

#endif