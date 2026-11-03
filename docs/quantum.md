# The `quantum.h` Library

`quantum.h` is a library inspired by the most important concepts of quantum mechanics. It is meant to experiment with concepts like a ternary logical value structure, linked values between variables and probabilities applied to a conjuct of objects. It is not related at all to actual quantum mechanics. In this document, we'll take a closer look to all the containers and functions that this library offers.

## Quantum States

A `quantum_state` is an enum class made to represent three distinct logical values: a false_v, a maybe_v and a true_v. This allows us to check for a state when something is not fully defined yet or to express three distincs states. Let's look at a practical example:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    int temperature{23};
    drk::quantum_state alert{};

    if(temperature > 30)
    {
        std::cout << "Temperature is too high!";
        alert = drk::quantum_state::true_v;
    } 
    else if (temperature < 10)
    {
        std::cout << "Temperature is too low!";
        alert = drk::quantum_state::false_v;
    }
    else
    {
        std::cout << "Temperature is OK!";
        alert = drk::quantum_state::maybe_v;
    }

    return 0;
}
```

## Ternary Quantum

A `ternary_quantum` is a class that stores an object and a `quantum_state`, relating both in only one structure. Its default quantum_state is `quantum_state::true_v`. Let's look at the previous example made for `quantum_state`, but now applying a `ternary_quantum`:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    drk::ternary_quantum<int> temperature{23};

    if(temperature.get_value() > 30)
    {
        std::cout << "Temperature is too high!";
        temperature.change_state(drk::quantum_state::true_v);
    } 
    else if (temperature.get_value() < 10)
    {
        std::cout << "Temperature is too low!";
        temperature.change_state(drk::quantum_state::false_v);
    }
    else
    {
        std::cout << "Temperature is OK!";
        temperature.change_state(drk::quantum_state::maybe_v);
    }

    return 0;
}
```

Also, you can use the `is_defined()` method to check if the `quantum_state` is diferent from `quantum_state::maybe_v`. This allows us to make a system like this:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    drk::ternary_quantum<int> temperature{23, drk::quantum_state::maybe_v};

    if(!temperature.is_defined()) {
        std::cout << "Temperature is OK!"; // Prints this
    }
    else {
        std::cout << "Temperature has surpassed the limits!";
    }

    return 0;
}
```

## Linked Quantum

A `linked_quantum` is a class that stores an owned value and a shared value with another `linked_quantum` of the same type. You can have multiple `linked_quantum`s linked to each other. This allows us to have multiple objects that share the same state, making any of the `linked_quantum`s in the tree able to modify the shared value of the main node. Let's look at a simple example:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    drk::linked_quantum<int, int> sensor_a{30};
    drk::linked_quantum<int, int> sensor_b{16};
    drk::linked_quantum<int, int> sensor_c{24, 35}; // This one has a shared value of 35

    sensor_a.link(sensor_b); // a is linked to b
    sensor_b.link(sensor_c); // b is linked to c

    sensor_a.set_linked_v(40); // a changes the shared value to 40

    std::cout << sensor_a.get_linked_v() << ' ';
    std::cout << sensor_b.get_linked_v() << ' ';
    std::cout << sensor_c.get_linked_v() << '\n';

    // This prints out: 40 40 40
    // All the owned values stay the same

    return 0;
}
```

If you don't want to have a relationship with another `linked_quantum` anymore, you can use the `reset()` method to make that relationship dissapears:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    drk::linked_quantum<int, char> sensor_a{30, 'a'};
    drk::linked_quantum<int, char> sensor_b{16, 'b'};
    drk::linked_quantum<int, char> sensor_c{24, 'c'};
    drk::linked_quantum<int, char> sensor_d{50, 'd'};

    sensor_a.link(sensor_b); // a is linked to b
    sensor_b.link(sensor_c); // b is linked to c
    sensor_d.link(sensor_c); // d is linked to c as well

    sensor_a.set_linked_v('e'); // a changes the shared value of c to 'e'
    sensor_b.reset(); // The relationship of b with c is broken, so a also losses access to c's value

    sensor_a.set_linked_v('f'); // a changes the shared value of b to 'f'

    std::cout << sensor_a.get_linked_v() << ' ';
    std::cout << sensor_b.get_linked_v() << ' ';
    std::cout << sensor_c.get_linked_v() << ' ';
    std::cout << sensor_d.get_linked_v() << '\n';

    // This prints out: f f e e

    return 0;
}
```

One thing to take into consideration is to avoid cyclic relationships at all cost. Even though the class has a way to avoid making these kind of mistakes, it's always better to avoid them if you notice it to prevent pointer errors.

## Uncertain Quantum

An `uncertain_quantum` is a class that stores an undefined ammount of objects which are choosen at random through a uniform or pounded distribution. You can change the kind of distribution at will. This allows us to have a randomized selection of values in a single structure. Let's look at an example:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    drk::uncertain_quantum<int> sensor{{1, 2, 3, 4, 5}};

    std::cout << sensor.observe();

    // This prints out a random number of the vector with the same probability

    return 0;
}
```

To make an `uncertain_quantum` have a pounded distribution of probabilities, you must define a second vector of probabilities in the constructor or through the method `make_pounded()`:

```C++
#include "quantum.h"
#include <iostream>

int main(){

    drk::uncertain_quantum<int> sensor_a{{1, 2, 3}, {1, 1, 8}};
    drk::uncertain_quantum<int> sensor_b{{1, 2, 3}};

    sensor_b.make_pounded({1, 1, 8}); // Defines de probabilities vector

    std::cout << sensor_a.observe();
    std::cout << sensor_b.observe();

    // The result for both should be pretty similar
    // Both are the exact same structure but with differente name
    // the value 3 has an 80% change of appear an the other two only a 10%

    return 0;
}
```

A common mistake that can appear when defining an `uncertain_quantum` is defining one with a pounded probability with a different size of the values stored:

```C++
drk::uncertain_quantum<int> sensor_a{{1, 2, 3}, {1, 1}}; // A value is missing!
```

In this case, the missing value is going to be replaced for a `0` and will never appear when calling the `observe()` method. To avoid this, the specialized `strict_uncertain` was created.

#### Strict Uncertain

If the owned values and the pounded probability vectors are different in size, the vector will thorw an exception of type `drk::incoherent_size`:

```C++
drk::uncertain_quantum<int> sensor_a{{1, 2, 3}, {1, 1}}; // Valid code (Should avoid)
drk::strict_uncertain<int> sensor_b{{1, 2, 3}, {1, 1}}; // Will throw an exception!
```

This is useful to always avoid some errors that, otherwise, we wouldn't notice right away.

## Conclusion

In general, the `quantum.h` library offers alternatives to some problems that you might encounter while working on projects that depend of more than a boolean, shared values or probabilities. At the end, it is still a fun project that was made for experimental purposes. All usage of this remains at the user's discretion.

## Licenses

The work here was written by Derek himself. The code of this repository is open sourced under the MIT License.