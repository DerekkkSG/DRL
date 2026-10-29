# DRL - Derek's Restricted Library

![C++20](https://img.shields.io/badge/Estándar-C%2B%2B20-00599C?style=for-the-badge&logo=c%2B%2B)
![Estado](https://img.shields.io/badge/Estado-En_Desarrollo-brightgreen?style=for-the-badge)

DRL es un proyecto personal liderado por Derek Salcido centrado en la creación de librerías propias en C++20 para uso personal o público. Esta biblioteca busca experimentar con formas creativas de implementar y diseñar astracciones propias en C++20 mediante el uso de plantillas.

Estas implementaciones estan hechas con fines de experimentacion y diseño de software sin necesariamente intentar resolver problemas reales, aunque se pueden utilizar al gusto del usuario para solucionar problemas personales que se puedan presentar.

## Contenido

DRL incluye por el momento una sola librería, aunque con la visión de agregar muchas más en un futuro:

* **quantum.h**: Es una librería inspirada por los conceptos fundamentales de la mecánica cuántica, intentando implementar de la manera más clara y sencilla posible conceptos como la incertidumbre o superposición cuántica sin intentar resolver problemas realmente relacionados con estos temas.

#### Características de quantum.h

* `drk::ternary_quantum<T>`: Almacena un valor cualquiera junto a un estado lógico ternario: false_v, maybe_v y true_v. Su estado logico por defecto es true_v.
* `drk::linked_quantum<Own, Linked>`: Almacena dos valores distintos: uno propio y otro compartido entre distintos objetos del mismo tipo.
* `drk::uncertain_quantum<T>`: Almacena varios valores, los cuales se pueden seleccionar aleatoriamente mediante una distribución uniforme o probabilidades ponderadas.
    - `drk::strict_uncertain<T>`: Es un **drk::uncertain_quantum**, pero con la diferencia de que éste no te permite tener un vector de valores de distinto tamaño al de probabilidades ponderadas, soltando una excepción de tipo std::range_error si se incumple esta norma.

Además, **quantum.h** cuenta con funciones especializadas para crear cada estructura, así como los tres estados lógicos distintos, entre otros. Algunas de las funciones o clases se encuentran dentro del espacio de nombres **drk::quantum**.

## Requisitos

* Un compilador que soporte C++20.
* No requiere herramientas externas como CMake o bibliotecas de terceros.

## Instalación y Uso

La DRL puede utilizarse directamente desde sus archivos de encabezado presentes en el folder `drk/`. Para agregarlo a tu proyecto:

1. Descarga o clona el repositorio.
2. Incluye "drk/quantum.h" en tu proyecto.
3. Compila tu programa con previo soporte a C++20.

```C++
//Ejemplo de implementacion
#include "drk/quantum.h"

#include <iostream>

int main() {

    drk::ternary_quantum<int> var {
        42,                          /* Valor almacenado */
        drk::quantum_state::maybe_v  /* Estado logico */
    };

    std::cout << var.get_value() << '\n';
    std::cout << var.get_state() << '\n';

    return 0;
}
```
Output esperado:
```bash
42
1
```

Todas las funciones, estructuras o datos presentes dentro de la DRL se encuentran bajo el espacio de nombres **drk**.

## Compilacion

Para compilar cualquier proyecto en la que se use la DRL, la compilacion se realiza de la siguiente manera para cada caso:

```bash
#Para un proyecto propio situado en la raiz
g++ -std=c++20 main.cpp -o main

#Para compilar los tests o examples
g++ -std=c++20 tests/readme_example.cpp -o readme_example
```
En caso de tener un compilador distinto a gcc, se sustituiría por el compilador de preferencia  compatible con C++20 (ej: clang, MSVC).
Para ejecutar el programa resultante en Windows, se utiliza el siguiente comando:
```powershell
.\archivo.exe
```

## Licencia

Este proyecto se distribuye bajo la licencia MIT.