#include "../drk/quantum.h"

#include <iostream>
#include <cassert>

using namespace std;

#define DEBUG_DRL
#ifndef DEBUG_DRL

// Main testing scene
int main(){

    drk::uncertain_quantum<int> unc_a{{1, 2, 3, 4, 5}};

    std::vector<int> p{1, 2, 3, 4, 5};
    drk::uncertain_quantum<int> unc_b{std::move(p)};

    drk::strict_uncertain<int> unc_c{{1, 2, 3, 4, 5}};

    unc_a.change_values({1, 2, 3});

    // 100% de probabilidad al último elemento
    drk::uncertain_quantum<int> unc_d{{10, 20, 99}, {0, 0, 100}};
    assert(unc_d.observe() == 99);
    assert(drk::auto_observe(unc_d) == 99);

    // Cambiar a uniforme ignora los pesos y permite que salgan los demás
    unc_d.make_uniform();
    bool validator = false;
    for (int i = 0; i < 50; ++i) {
        if (unc_d.observe() != 99) {
            validator = true;
            break;
        }
    }
    assert(validator);

    // Cambiamos a 2 valores aunque había 3 pesos (no debe lanzar excepción)
    unc_d.make_pounded({10, 20, 70});
    unc_d.change_values({5, 15});
    int obs = unc_d.observe();
    assert(obs == 5 || obs == 15);

    // Probando a strict_uncertain
    {
        // Caso válido
        drk::strict_uncertain<int> strict_a{{100, 200}, {0, 100}};
        assert(strict_a.observe() == 200);

        // Error 1: El constructor con tamaños distintos debe lanzar un drk::incoherent_size
        bool validator_a = false;
        try {
            drk::strict_uncertain<int> invalido{{1, 2, 3}, {50, 50}};
        } catch (const drk::incoherent_size&) {
            validator_a = true;
        }
        assert(validator_a);

        // Error 2: uso incorrecto/incoherente de change_values()
        bool validator_b = false;
        try {
            strict_a.change_values({1, 2, 3}); // Antes tenía 2 pesos, ahora 3 valores
        } catch (const drk::incoherent_size&) {
            validator_b = true;
        }
        assert(validator_b);

        // Error 3: Construido uniforme sin pesos y luego se cambia a make_pounded() sin pasar pesos
        bool validator_c = false;
        try {
            drk::strict_uncertain<int> strict_b{{1, 2, 3}};
            strict_b.make_pounded();
        } catch (const drk::incoherent_size&) {
            validator_c = true;
        }
        assert(validator_c);
    }

    // Poniendolo a prueba con la semantica de movimiento y sus iteradores
    drk::uncertain_quantum<int> unc_e{};
    assert(unc_e.observe() == 0);

    // Con todos los pesos en cero, devuelve un T{}, evitando asi errores
    drk::uncertain_quantum<int> unc_f{{10, 20, 30}, {0, 0, 0}};
    assert(unc_f.observe() == 0);

    // Probando los Iteradores para modificar elementos internos
    drk::uncertain_quantum<int> unc_g{{1, 2, 3}};
    for (auto& val : unc_g) {
        val *= 10;
    }
    unc_g.make_pounded({0, 100, 0});
    assert(unc_g.observe() == 20);

    // Constructor por movimiento
    auto unc_h = std::move(unc_g);
    assert(unc_h.observe() == 20);
    assert(unc_g.observe() == 0); // El objeto movido queda en estado por defecto

    auto unc_i = drk::make_strict_uncertain<int>({7, 8}, {100, 0});
    assert(unc_i.observe() == 7);

    return 0;
}

#endif