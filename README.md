# DRL - Derek's Restricted Library

![C++20](https://img.shields.io/badge/Estándar-C%2B%2B20-00599C?style=for-the-badge&logo=c%2B%2B)
![Estado](https://img.shields.io/badge/Estado-En_Desarrollo-brightgreen?style=for-the-badge)
![Formato](https://img.shields.io/badge/Formato-Header_Only-purple?style=for-the-badge)

DRL es un proyecto personal inspirado por la STL y liderado por Derek Salcido centrado en la creación de **librerías header-only** propias en C++20 para uso personal o público. 

Esta biblioteca busca experimentar con formas creativas de implementar y diseñar astracciones propias mediante el uso de plantillas. Ésta tiene fines experimentales y no necesariamente intenta resolver problemas reales ni crear una estructura igual de robusta que la librería estándar. Sin embargo, se pueden usar libremente para solucionar posibles problemas en proyectos personales que se puedan presentar.

## Contenido

DRL incluye por el momento una sola librería, aunque con la visión de agregar muchas más en un futuro:

* **quantum.h**: Es una librería inspirada por los conceptos fundamentales de la mecánica cuántica, intentando implementar de la manera más clara y sencilla posible conceptos como la incertidumbre o superposición cuántica sin intentar resolver problemas realmente relacionados con estos temas.

#### Características de quantum.h

* `drk::ternary_quantum<T>`: Almacena un valor cualquiera junto a un estado lógico ternario: false_v, maybe_v y true_v. Su estado logico por defecto es true_v.
* `drk::linked_quantum<Own, Linked>`: Almacena dos valores distintos: uno propio y otro compartido entre distintos objetos del mismo tipo que tu decidas vincular.
* `drk::uncertain_quantum<T>`: Almacena varios valores, los cuales se pueden seleccionar aleatoriamente mediante una distribución uniforme o probabilidades ponderadas.
    - `drk::strict_uncertain<T>`: Es una especialización de **drk::uncertain_quantum** que no te permite tener un vector de valores de distinto tamaño al de probabilidades ponderadas, soltando una excepción de tipo *drk::incoherent_size* si se incumple esta norma.

Además, **quantum.h** cuenta con funciones especializadas para crear cada estructura, así como los tres estados lógicos distintos, entre otros. Algunas de las utilidades se encuentran dentro del espacio de nombres **drk::quantum**.

## Requisitos

* Un compilador que soporte completamente C++20.
* No requiere herramientas externas como CMake o bibliotecas de terceros.

## Instalación y Uso

La DRL puede utilizarse directamente desde sus archivos de encabezado presentes en el folder `drk/`. Para agregarlo a tu proyecto:

1. Clona el repositorio o descarga el archivo de cabezera de interés.
2. Incluye el archivo de cabezera de interes en tu proyecto.
3. Compila tu programa con previo soporte a C++20.

Aquí un ejemplo de uso e implementación para cualquier libreria dentro de la DRL romando como ejemplo a `quantum.h`:

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
    std::cout << static_cast<int>(var.get_state()) << '\n';

    return 0;
}
```
Output esperado:
```bash
42
1
```

Todas las funciones, estructuras de datos o utilidades presentes dentro de la DRL se encuentran bajo el espacio de nombres **drk**.

## Compilacion

Para compilar cualquier proyecto en la que se use la DRL, la compilacion se realiza de la siguiente manera para cada caso:

```bash
#Para un proyecto propio situado en la raiz
g++ -std=c++20 main.cpp -o main

#Para compilar los tests o examples
g++ -std=c++20 examples/readme_example.cpp -o readme_example
```
En caso de tener un compilador distinto a gcc, se sustituiría por el compilador de preferencia compatible con C++20 (ej: clang, MSVC).
Para ejecutar el programa resultante en Windows, se utiliza el siguiente comando:
```powershell
.\archivo.exe
```

## Planes Futuros

## Licencia

Este proyecto se distribuye bajo la licencia MIT. Para más información, leer LICENSE.