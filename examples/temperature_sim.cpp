#include "../drk/quantum.h"
#include "../drk/aliases.h"

#include <iostream>
#include <string>

#ifndef DEBUG_DRL

int main() {
    // Variables utilizadas para la simulacion de un sensor de temperatura: Un controlador y un ventilador
    drk::linked<std::string, int> controlador{"Control Termico", 75};
    drk::linked<std::string, int> ventilador{"Ventilador"};
    ventilador.link(controlador);

    // El sistema entra en modo ahorro y reduce el umbral compartido a 60 C
    controlador.set_shared(60);

    // Termistor NTC con ruido ponderado (50% normal, 30% tibio, 20% alerta)
    drk::strict_uncertain<int> termistor{
        {45, 58, 95}, // Valores posibles
        {50, 30, 20}  // Probabilidades impuestas
    };

    // Se crea una variable para capturar las lecturas y el estado en el que se encuentra el sensor
    drk::ternary<int> sensor{45, drk::true_v};

    std::cout << "Umbral termico: " << ventilador.shared() << " C\n\n";

    for (int i = 1; i <= 5; i++) {
        int temp_actual = termistor.observe();

        // Si la temperatura salta mas de 25 C de golpe, es interferencia (maybe_v)
        if (temp_actual - sensor.get_value() > 25) {
            sensor.change_state(drk::maybe_v);
        } else if (temp_actual <= ventilador.shared()) {
            sensor.change_state(drk::true_v);
        } else {
            sensor.change_state(drk::false_v);
        }

        sensor.set_value(temp_actual);

        std::cout << "Ciclo " << i << " | sensor: " << sensor.get_value() << " C -> ";
        if (!sensor.is_defined()) {
            std::cout << " ??? Interferencia detectada\n";
        } else if (sensor.get_state() == drk::true_v) {
            std::cout << " Operacion normal.\n";
        } else {
            std::cout << " !!! Alerta: supera los " << ventilador.shared() << " C\n";
        }
    }

    return 0;
}

#endif