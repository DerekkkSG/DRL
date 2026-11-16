# La Librería `quantum.h`

`quantum.h` es una librería inspirada por los conceptos más importantes de la mecánica cuántica. Su objetivo es experimentar con conceptos como una estructura lógica ternaria, valores enlazados entre variables y probabilidades aplicadas a un conjunto de objetos. No está relacionada en nada con la mecánica cuántica de verdad. En este documento, veremos todos los contenedores, funciones y utilidades que esta librería ofrece.

## Quantum States

Un `quantum_state` es un enum class hecho para representar tres valores lógicos distintos: un false_v, un maybe_v y un true_v. Esto nos permite verificar si un valor aún no está completamente definido o para ezpresar tres estados distintos. Veamos un ejemplo:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    int temperature{23};
    drk::quantum_state alert{};

    if(temperature > 30)
    {
        std::cout << "La temperatura es demasiado alta!";
        alert = drk::quantum_state::true_v;
    } 
    else if (temperature < 10)
    {
        std::cout << "La temperatura es demasiado baja!";
        alert = drk::quantum_state::false_v;
    }
    else
    {
        std::cout << "La temperatura está estable!";
        alert = drk::quantum_state::maybe_v;
    }

    return 0;
}
```

## Ternary Quantum

Un `ternary_quantum` es una clase que almacena un objeto y un `quantum_state`, relacionando ambos en una misma estructura. Su quantum_state por defecto es `quantum_state::true_v`. Veremos al mismo ejemplo aplicado para `quantum_state`, pero ahora aplicando un `ternary_quantum`:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    drk::ternary_quantum<int> temperature{23};

    if(temperature.get_value() > 30)
    {
        std::cout << "La temperatura es demasiado alta!";
        temperature.change_state(drk::quantum_state::true_v);
    } 
    else if (temperature.get_value() < 10)
    {
        std::cout << "La temperatura es demasiado baja!";
        temperature.change_state(drk::quantum_state::false_v);
    }
    else
    {
        std::cout << "La temperatura está estable!";
        temperature.change_state(drk::quantum_state::maybe_v);
    }

    return 0;
}
```

A su vez, puedes utilizar el método `is_defined()` para verificar si el `quantum_state` es diferente de `quantum_state::maybe_v`. Esto nos permite crear un sistema así:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    drk::ternary_quantum<int> temperature{23, drk::quantum_state::maybe_v};

    if(!temperature.is_defined()) {
        std::cout << "La temperatura está estable!"; // Imprime esto
    }
    else {
        std::cout << "La temperatura ha superado los límites!";
    }

    return 0;
}
```

## Linked Quantum

Un `linked_quantum` es una clase que almacena un valor propio y uno compartido con otro `linked_quantum` del mismo tipo. Puedes tener múltiples `linked_quantum`s enlazados unos a otros. Esto nos permite tener múltiples objetos que comparten el mismo estado, haciendo a cualquiera de los `linked_quantum`s en el árbol capaces de modificar el valor compartido del nodo principal. Veamos a un ejemplo sencillo:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    drk::linked_quantum<int, int> sensor_a{30};
    drk::linked_quantum<int, int> sensor_b{16};
    drk::linked_quantum<int, int> sensor_c{24, 35}; // Este tiene un valor compartido de 35

    sensor_a.link(sensor_b); // a está enlazado a b
    sensor_b.link(sensor_c); // b está enlazado a c

    sensor_a.set_shared(40); // a cambia el valor compartido a 40

    std::cout << sensor_a.shared() << ' ';
    std::cout << sensor_b.shared() << ' ';
    std::cout << sensor_c.shared() << '\n';

    // Esto imprime: 40 40 40
    // Todos los valores propios permanecen igual

    return 0;
}
```

Si ya no quieres mantener una relación con otro `linked_quantum`, puedes usar el método `reset()` para hacer que esa relación desaparezca:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    drk::linked_quantum<int, int> sensor_a{30, 2};
    drk::linked_quantum<int, int> sensor_b{16, 5};

    sensor_a.link(sensor_b); // a se enlaza a b

    std::cout << sensor_a.shared() << ' ';
    std::cout << sensor_b.shared() << '\n'; 
    // Esto imprime: 5 5

    sensor_a.reset(); // Rompe la relacion con b

    std::cout << sensor_a.shared() << ' ';
    std::cout << sensor_b.shared() << '\n'; 
    // Esto imprime: 0 5
    // Al hacer reset(), el valor compartido se reinicia junto a su relacion
    // b mantiene su estado como si nada hubiera pasado

    return 0;
}
```

Como se vio en el segundo ejemplo, `reset()` solo rompe la relacion con su `linked_quantum` compartido: toda la red de nodos permanece como deberia con sus respectivas relaciones sin ser afectadas por el `reset()` de un `linked_quantum` cualquiera. Para entender mejor el concepto, veamos un ejemplo un poco mas compejo: 

```C++
#include "quantum.h"
#include <iostream>

int main(){

    drk::linked_quantum<int, char> sensor_a{30, 'a'};
    drk::linked_quantum<int, char> sensor_b{16, 'b'};
    drk::linked_quantum<int, char> sensor_c{24, 'c'};
    drk::linked_quantum<int, char> sensor_d{50, 'd'};

    sensor_a.link(sensor_b); // a está enlazado a b
    sensor_b.link(sensor_c); // b está enlazado a c
    sensor_d.link(sensor_c); // d también está enlazado a c

    sensor_a.set_shared('e'); // a cambia el valor compartido de c a 'e'

    sensor_b.reset(); 
    // La relación de b con c se rompió.
    // Entonces a también perdió por consecuencia acceso al valor de c

    sensor_a.set_shared('f'); // a cambia el valor compartido de b a 'f'

    std::cout << sensor_a.shared() << ' ';
    std::cout << sensor_b.shared() << ' ';
    std::cout << sensor_c.shared() << ' ';
    std::cout << sensor_d.shared() << '\n';

    // Esto imprime: f f e e

    return 0;
}
```

Un tema a tomar en consideración es el siempre evitar crear relaciones circulares entre `linked_quantum`s. Aunque la propia clase tiene formas de evitar caer en estos errores, siempre será mejor evitarlos si los notas para prevenir problemas con recursión infinita.

## Uncertain Quantum

Un `uncertain_quantum` es una clase que almacena una cantidad variabel de objetos os cuales son escogidos aleatoriamente a través de una distribución uniforma o ponderada. Puedes cambiar el tipo de distribución cuando desees. Esto nos permite tener una serie de valores aleatorios en una sola estructuraa. Veamos un ejemplo:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    drk::uncertain_quantum<int> sensor{{1, 2, 3, 4, 5}};

    std::cout << sensor.observe();

    // Esto imprime un valor aleatorio del vector con la misma probabilidad para cada uno

    return 0;
}
```

Para hacer que un `uncertain_quantum` tenga una distribución de probabilidades ponderadas, necesitas definir un segundo vector de probabilidades asignadas en el constructor o a través del método `make_pounded()`:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    drk::uncertain_quantum<int> sensor_a{{1, 2, 3}, {1, 1, 8}};
    drk::uncertain_quantum<int> sensor_b{{1, 2, 3}};

    sensor_b.make_pounded({1, 1, 8}); // Define el vector de probabilidades

    std::cout << sensor_a.observe();
    std::cout << sensor_b.observe();

    // El resultado para ambos debe ser muy similar
    // Prácticamente, ambos son la misma estructura pero con distinto nombre
    // El número 3 tiene un 80% de probabiliadd de aparecer y los otros dos apenas un 10%

    return 0;
}
```

Un error común que puede aparecer cuando defines un `uncertain_quantum` es definir un vector de probabilidades ponderadas de tamaño distinto al de los valores asignados:

```C++
drk::uncertain_quantum<int> sensor_a{{1, 2, 3}, {1, 1}}; // Hace falta un valor!
```

En este caso, el valor faltante va a ser remplazado por un `0` y nunca va a aparecer al llamar el método `observe()`. Para evitar esto, la especialización `strict_uncertain` fue creada.

#### Strict Uncertain

Si el vetor de valores y de probabilidades ponderadas cuentan con un tamaño distinto, el vector va a tirar una exxcepción de tipo `drk::incoherent_size`:

```C++
drk::uncertain_quantum<int> sensor_a{{1, 2, 3}, {1, 1}}; // Código válido (Deberías evitarlo)
drk::strict_uncertain<int> sensor_b{{1, 2, 3}, {1, 1}};  // Va a tirar una excepión!
```

Esto es útil para siempre tratar de evitar estos errores que, de otra manera, no habríamos notado en el momento.

## Conclusión

En general, la librería `quantum.h` ofrece alternativas a unos problemas que podrías encontrarte mientras trabajas que dependen de más de un simple boleano, de valores compartidos o de probabilidades. Al final, este sigue siendo un proyecto hecho por diversión y para experimentr con el lenguaje. El uso de esta librería siempre queda a discreción del usuario.

## Licencia

El documento fue escrito por el mismo Derek. El código de este repositorio es oper source bajo la Licensia MIT. Para más información, leer LICENSE.