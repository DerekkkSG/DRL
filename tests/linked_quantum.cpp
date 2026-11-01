#include "../drk/quantum.h"

#include <iostream>
#include <utility>
#include <memory>

using namespace std;

#ifndef DEBUG_DRL
int main(){

    /* Usando los constructores por parametro disponibles */
    drk::linked_quantum<int, float> linked_a{};
    drk::linked_quantum<int, float> linked_b{22};
    drk::linked_quantum<int, float> linked_c{24, 3.2f};

    // Probando la funcion set_own_v()
    linked_a.set_own_v(5);

    // Probando el metodo link() uno detras del otro.
    linked_a.link(linked_b);
    linked_b.link(linked_c);

    // Probando el enlace de cada uno de los linked_quantums.
    // Al estar todos enlazados, deben modificarse todos a la vez.
    linked_a.set_linked_v(5.6f);
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    linked_b.set_linked_v(2.3f);
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    linked_c.set_linked_v(24.5f);
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    // Probando el metodo reset().
    // Al estar linked_a enlazado a linked_b, ambos deben de tener un valor compartido igual.
    linked_b.reset();
    linked_b.set_linked_v(7.8f);
    // Probando la sobrecarga del operador = que permite comportarse como una variable cualquiera
    // Es equivalente al metodo set_own_v()
    linked_b = 9;
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    linked_b = {45, 9.3f}; // Reasignando un linked_quantum a linked_b
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    // Probando la asignacion por movimiento
    linked_b = std::move(linked_c);
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    // Probando la funcion drk::make_linked()
    linked_c = drk::make_linked(4, 5.6f);
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    // Comprobando una vez mas las asignaciones por movimiento
    linked_c.link(linked_b);
    linked_a = std::move(linked_b);
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    // Comprobando si tiene algun link cada estruxtura con is_linked().
    // Gracias a las operaciones por movimiento, el output debe ser: 0 0 1
    cout << linked_a.is_linked() << ' ' << linked_b.is_linked() << ' ' << linked_c.is_linked() << '\n';

    return 0;
}
#endif