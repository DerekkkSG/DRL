#include "../drk/quantum.h"

// Esta estructura es demasiado estricta con las copias debido a su sistema de punteros.
// Siempre utiliza la estructura en lugares donde sepas que siempre se mantendra la misma direccion de memoria.
// De lo contrario, podrias tener errores con contenedores como std::vector, etc.

#include <iostream>
#include <utility>
#include <memory>
#include <cassert>
#include <optional>

using namespace std;

#ifndef DEBUG_DRL

// Function dependencies for iostream compatibility
std::ostream& operator<<(std::ostream& ostream, const drk::quantum_state& state);

template<typename Own, typename Linked>
std::ostream& operator<<(std::ostream& ostream, drk::linked_quantum<Own, Linked> quantum);

struct Vector2 {
    double x;
    double y;

    Vector2() = default;
    Vector2(double _x, double _y) : x(_x), y(_y) {}
};

// Main testing scene
int main(){

    // Usando los constructores por parametro disponibles
    drk::linked_quantum<int, float> linked_a{};
    drk::linked_quantum<int, float> linked_b{22};
    drk::linked_quantum<int, float> linked_c{24, 3.2f};

    // Probando la funcion set_owned()
    linked_a.set_owned(5);

    // Probando el metodo link() uno detras del otro.
    linked_a.link(linked_b);
    linked_b.link(linked_c);
    // Probando que el resultado se mantenga igual al hacer link() sobre si mismo
    linked_a.link(linked_a);

    // Ambos punteros son distintos, ya que a esta enlazado a b y b a c
    assert(linked_a.linked_ptr() != linked_b.linked_ptr());

    // Probando el enlace de cada uno de los linked_quantums.
    // Al estar todos enlazados, deben modificarse todos a la vez.
    linked_a.set_shared(5.6f);
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    linked_b.set_shared(2.3f);
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    linked_c.set_shared(24.5f);
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    // Probando el metodo reset().
    // Al estar linked_a enlazado a linked_b, ambos deben de tener un valor compartido igual.
    linked_b.reset();
    linked_b.set_shared(7.8f);
    // Probando la sobrecarga del operador = que permite comportarse como una variable cualquiera
    // Es equivalente al metodo set_owned()
    linked_b = 9;
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    // Reasignando un linked_quantum a linked_b
    linked_b.set_owned(45);
    linked_b.set_shared(9.3f); 
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    // Probando la asignacion por movimiento
    linked_b = std::move(linked_c);
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    auto linked_checker = drk::make_linked<int, float>(); // El estado por defecto de un linked_quantum

    assert(linked_c == linked_checker);

    // Probando la funcion drk::make_linked()
    linked_c = drk::make_linked(4, 5.6f);
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    // Comprobando una vez mas las asignaciones por movimiento
    linked_c.link(linked_b);
    linked_a = std::move(linked_b);
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    assert(linked_b == linked_checker);

    // Comprobando si tiene algun link cada estruxtura con is_linked().
    // Gracias a las operaciones por movimiento, el output debe ser: 0 0 1
    cout << linked_a.is_linked() << ' ' << linked_b.is_linked() << ' ' << linked_c.is_linked() << '\n';

    // Tratando de modificar el valor compartido por movimiento
    std::shared_ptr<int> forward_checker = make_shared<int>(7);
    auto linked_obj = drk::make_linked<int, std::shared_ptr<int>>();
    linked_obj.set_shared(std::move(forward_checker));

    assert(forward_checker == nullptr);

    // Probando el comportamiento del linked_quantum una vez un objeto se destruye
    {
        auto linked_d = drk::make_linked<int, float>(5, 7.8f);
        linked_c.link(linked_d);
        cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';
    }

    // Como se espera, linked_c deja de estar enlazado ya que el objeto linked_d ha sido destruido
    cout << linked_a.is_linked() << ' ' << linked_b.is_linked() << ' ' << linked_c.is_linked() << '\n';

    // Intentando crear una relacion ciclica indirecta entre linked_quantums
    linked_b.set_shared(19.6f);
    linked_a.link(linked_c);
    linked_c.link(linked_b);
    linked_b.link(linked_a);

    // El Output de is_linked() para cada linked_quantum debe 1 0 1, ya que la relacion previa de linked_b a linked_a no es posible
    // debido a que linked_a estaria apuntando indirectamente a linked_c.
    // No se permiten relaciones ciclicas.
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';
    cout << linked_a.is_linked() << ' ' << linked_b.is_linked() << ' ' << linked_c.is_linked() << '\n';

    // Probando una relacion ciclica directa entre linked_quantums
    linked_a.link(linked_b);
    linked_b.link(linked_a);

    // El resultado debe de ser el mismo que el anterior, solo con la diferencia de que linked_a ya no apunta a linked_c, 
    // pero a linked_b directamente. Por la misma razon, linked_b permanece sin relacion alguna: relacion ciclica.
    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';
    cout << linked_a.is_linked() << ' ' << linked_b.is_linked() << ' ' << linked_c.is_linked() << '\n';

    // Probando la estructura con otros contenedores como std::optional
    std::optional<drk::linked_quantum<int, float>> opt_linked_a;
    assert(!opt_linked_a.has_value());

    auto opt_linked_b = std::make_optional(drk::make_linked(3, 4.5f));

    assert(opt_linked_b.has_value());
    assert(!opt_linked_b->is_linked());
    assert(opt_linked_b->owned() == 3);

    drk::linked_quantum<int, float> linked_e{2, 10.0f};
    drk::linked_quantum<int, float> linked_f{6};

    linked_f.link(linked_e);

    opt_linked_a = std::move(linked_e);

    assert(opt_linked_a.has_value());
    assert(linked_f.is_linked());
    assert(linked_f.shared() == opt_linked_a->shared());

    drk::linked_quantum<int, float> linked_g{42};

    {
        std::optional<drk::linked_quantum<int, float>> opt_linked_c{std::in_place, 100, 1.0f};
        linked_g.link(*opt_linked_c);
        assert(linked_g.is_linked());
    }

    assert(!linked_g.is_linked());

    linked_a.reset(); // Eliminando las relaciones previas con otros linked_quantums
    linked_b.link(linked_a);
    linked_c.link_to_root(linked_b); // Sube hasta la raiz y guarda la direccion de linked_a

    linked_c.set_shared(983.1f);

    // A diferencia de link(), aqui si deben de ser los mismos punteros independientemente de donde provenga la relacion
    assert(linked_b.linked_ptr() == linked_c.linked_ptr());

    cout << linked_a << ' ' << linked_b << ' ' << linked_c << '\n';

    // Probando su compatibilidad con objetos mediante los operadores * y ->
    drk::linked_quantum<int, Vector2> linked_j{};
    Vector2 object_checker{7, 8};

    linked_j->x = 7;
    linked_j->y = 8;

    assert((*linked_j).x == 7 && (*linked_j).y == 8);

    return 0;
}

// Function declaration
std::ostream& operator<<(std::ostream& ostream, drk::quantum_state state){
    switch(state){
        case drk::quantum_state::false_v: ostream << static_cast<int>(drk::quantum_state::false_v); break;
        case drk::quantum_state::maybe_v: ostream << static_cast<int>(drk::quantum_state::maybe_v); break;
        case drk::quantum_state::true_v: ostream << static_cast<int>(drk::quantum_state::true_v); break;
        default: ostream << static_cast<int>(drk::quantum_state::false_v);
    }
    return ostream;
}

template<typename Own, typename Linked>
std::ostream& operator<<(std::ostream& ostream, drk::linked_quantum<Own, Linked> quantum){
    ostream << "{" << quantum.owned() << ", " << quantum.shared() << "}";
    return ostream;
}

#endif