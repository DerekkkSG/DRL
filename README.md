# DRL - Derek's Restricted Library

![C++20](https://img.shields.io/badge/Estándar-C%2B%2B20-00599C?style=for-the-badge&logo=c%2B%2B)
![Estado](https://img.shields.io/badge/Estado-En_Desarrollo-brightgreen?style=for-the-badge)

Proyecto personal liderado por Derek Salcido centrado en la creación de librerías propias en C++20 para uso personal o público. Éste busca experimentar con nuevas formas de relacionar valores con estados logicos o probabilidades impuestas a los datos.

## Características

DRL incluye por el momento una sola librería: **quantum.h**, con la visión de agregar muchas más en un futuro. Sus características principales son tres estructuras distintas:

* `drk::ternary_quantum<T>`: Almacena un valor cualquiera junto a un estado lógico ternario: false_v, maybe_v y true_v. Su estado logico por defecto es true_v.
* `drk::linked_quantum<Own, Linked>`: Almacena dos valores distintos: uno propio y otro compartido entre distintos objetos del mismo tipo.
* `drk::uncertain_quantum<T>`: Almacena varios valores, los cuales se pueden seleccionar aleatoriamente mediante una distribución uniforme o probabilidades ponderadas.
    - `drk::strict_uncertain<T>`: Es un **drk::uncertain_quantum**, pero con la diferencia de que éste no te permite tener un vector de valores de distinto tamaño al de probabilidades ponderadas, soltando una excepción de tipo std::range_error si se incumple esta norma.

Además, cuenta con funciones especializadas para crear cada estructura, así como los tres estados lógicos distintos y una función dedicada para obtenerlos de forma aleatoria. Todas éstas y más se encuentran dentro del espacio de nombres **drk::quantum**

## Requisitos

* Un compilador que soporte C++20.
* No requiere herramientas externas como CMake o bibliotecas de terceros.

## Instalación y Uso

La DRL puede utilizarse directamente desde sus archivos de encabezado presentes en el folder `drk/`. Para agregarlo a tu proyecto:

1. Descarga o clona el repositorio.
2. Incluye "drk/quantum.h" en tu proyecto.
3. Compila tu programa con previo soporte a C++20.

```
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
```
42
1
```

Todas las funciones, estructuras o datos presentes dentro de la DRL se encuentran bajo el espacio de nombres **drk**.

## Compilacion

Suponiendo que tu archivo lleva de nombre "main.cpp" y que está situado en la raiz del proyecto:

```
g++ -std=c++20 main.cpp -o main
```
En caso de tener un compilador distinto a gcc, se sustituiría por el compilador de preferencia  compatible con C++20 (ej: clang, MSVC).
Para ejecutar el programa en Windows, se utiliza el siguiente comando:
```
.\main.exe
```

## Licencia

Este proyecto se distribuye bajo la licencia MIT.