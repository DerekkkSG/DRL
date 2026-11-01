//Ejemplo de implementacion
#include "../drk/quantum.h"

#include <iostream>

#define DEBUG_DRL
#ifndef DEBUG_DRL
int main() {

    drk::ternary_quantum<int> var {
        42,                         /* Valor almacenado */
        drk::quantum_state::maybe_v /* Estado logico */
    };

    std::cout << var.get_value() << '\n';
    std::cout << var.get_state() << '\n';

    return 0;
}
#endif