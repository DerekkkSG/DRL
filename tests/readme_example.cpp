//Ejemplo de implementacion
#include "../drk/quantum.h"

#include <iostream>

int main() {

    drk::ternary_quantum<int> var {
        42,
        drk::quantum_state::maybe_v
    };

    std::cout << var.get_value() << '\n';
    std::cout << var.get_state() << '\n';

    return 0;
}