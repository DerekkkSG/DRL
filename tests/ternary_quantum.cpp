#include "../drk/quantum.h"

#include <iostream>
#include <utility>
#include <memory>

using namespace std;

int main(){

    /* Creando un ternary_quantum por defecto */
    drk::ternary_quantum<int> quantum1{};
    /* Creando un ternary_quantum por su constructor con dos parametros */
    drk::ternary_quantum<int> quantum2{34, drk::quantum_state::false_v};

    /* Definiendo el valor de quantum1 como la mitad de quantum2, probando el operador / */
    quantum1 = quantum2/2;

    /* Probando los metodos get_value() y get_state() */
    cout << "Valor de quantum1: " << quantum1.get_value() << " | Estado de quantum1: " << quantum1.get_state() << '\n';
    cout << "Valor de quantum2: " << quantum2.get_value() << " | Estado de quantum2: " << quantum2.get_state() << '\n';

    /* Probando los operadores sobrecargados. El estado no se modifica, solo el valor */
    cout << "Operador + de quantum1 y quantum2: " << quantum1 + quantum2 << '\n';
    cout << "Operador - de quantum1 y quantum2: " << quantum1 - quantum2 << '\n';
    cout << "Operador * de quantum1 y quantum2: " << quantum1 * quantum2 << '\n';
    /* Se imprime 0 en el operador / ya que son valores enteros los divididos */
    cout << "Operador / de quantum1 y quantum2: " << quantum1 / quantum2 << '\n';

    quantum1 *= quantum2;
    quantum1 -= quantum2;
    quantum1 /= quantum2;
    quantum1 += quantum2;

    cout << "Valor de quantum1: " << quantum1.get_value() << " | Estado de quantum1: " << quantum1.get_state() << '\n';

    /* Creando un quantum3 por el metodo drk::make_ternary sin parametros */
    auto quantum3 = drk::make_ternary<int>();
    /* Creando un quantum3 por el metodo drk::make_ternary con parametros */
    auto quantum4 = drk::make_ternary(5, drk::quantum_state::maybe_v);

    /* Se ve el resultado de 0 y _::true_v (2) al imprimirse al ser el valor por defecto */
    cout << "Valor de quantum3: " << quantum3.get_value() << " | Estado de quantum3: " << quantum3.get_state() << '\n';

    /* Moviendo quantum4 a quantum1. */
    quantum1 = std::move(quantum4);

    cout << "Valor de quantum1: " << quantum1.get_value() << " | Estado de quantum1: " << quantum1.get_state() << '\n';
    /* Despues del movimeinto, el  quantum4 debe de ser igual a quantum3. Probando el operador == */
    if(quantum3 == quantum4) cout << "quantum3 y quantum4 son iguales!\n";

    /* Probando el metodo is_defined() */
    if(!quantum1.is_defined()) cout << "Se necesita definir el valor de quantum1. Estado actual: _::maybe_v\n";

    /* Redefiniendo a quantum3 y probando crear un ternary_quantum por movimeinto */
    quantum3 = {-7, drk::quantum_state::maybe_v};
    drk::ternary_quantum<int> quantum5{std::move(quantum3)};

    cout << "Valor de quantum5: " << quantum5.get_value() << " | Estado de quantum5: " << quantum5.get_state() << '\n';
    /* quantum3 se queda con los valores por defecto */
    cout << "Valor de quantum3: " << quantum3.get_value() << " | Estado de quantum3: " << quantum3.get_state() << '\n';

    /* Se redefine quantum1 como quantum5 */
    quantum1 = quantum5;
    /* Probando los metodos change_state() y set_value() */
    quantum5.change_state(drk::quantum_state::true_v);
    quantum5.set_value(-90);

    cout << "Valor de quantum5: " << quantum5.get_value() << " | Estado de quantum5: " << quantum5.get_state() << '\n';

    /* Probando ternary_quantum con tipos de dato no primitivos */
    drk::ternary_quantum<shared_ptr<int>> quantum6{make_shared<int>(5)};
    drk::ternary_quantum<shared_ptr<int>> quantum7{nullptr};

    /* Probando la asignacion para valores no primitivos */
    quantum7 = quantum6;
    quantum7.change_state(drk::quantum_state::maybe_v);

    cout << "Valor de quantum6: " << *(quantum6.value) << " | Estado de quantum6: " << quantum6.get_state() << '\n';
    cout << "Valor de quantum7: " << *(quantum7.value) << " | Estado de quantum7: " << quantum7.get_state() << '\n';

    /*

    ERRORES Y CONFUSIONES COMUNES

    #Intentar asignar un ternary_quantum de dos distintos tipos directamente.
    quantum2 = quantum7; //Tomando como ejemplo el test actual

    #Intentar usar la funcion get_value() para valores no primitivos
    drk::ternary_quantum<std::shared_ptr<int>> quantum{make_shared<int>()};
    std::shared_ptr<int> ptrQuantum = quantum.get_value(); //Error de compilacion
    std::shared_ptr<int> ptrQuantum = quantum.value; //Forma correcta

    #Creer que los operadores (con excepcion de =) cambian el estado de un ternary_quantum
    drk::ternary_quantum<int> quantum1{5, drk::quantum_state::maybe_v};
    drk::ternary_quantum<int> quantum2{8};

    quantum1 *= quantum2; //El valor de quantum1 cambia a ser 40, pero el estado permanece como drk::quantum_state::maybe_v
    
    */

    return 0;
}