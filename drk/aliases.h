/*
------------------------------------------------------------------------------------------------------------------------
Copyright (c) 2026 Derek Salcido
Its use is under the terms of the MIT license. See LICENSE for details.

This library is for personal or public use, made by Derek himself.
Official support is limited to copies obtained from the main reppository.
Third-party distributions are not officialy supported.

If you get to use the library, please don't trust your life on it; it's not the best implemented.
------------------------------------------------------------------------------------------------------------------------
*/

/*
------------------------------------------------------------------------------------------------------------------------
SPECIAL NOTE:
aliases.h is a library that NEEDS to be added AFTER ALL THE OTHER DRL LIBRARIES.
If you do otherwise, the shortcuts will not be included.
------------------------------------------------------------------------------------------------------------------------
*/

#pragma once
#ifndef DEREKS_RESTRICTED_LIB_ALIASES_H
#define DEREKS_RESTRICTED_LIB_ALIASES_H

#if defined(_MSVC_LANG)
    #if _MSVC_LANG < 202002L
        #error "aliases.h is not compatible with your current version of C++. You need to compile with /std:c++20 at minimum."
    #endif
#else 
    #if __cplusplus < 202002L
        #error "aliases.h is not compatible with your current version of C++. You need to compile with -std=c++20 at minimum."
    #endif
#endif

#include <concepts>

namespace drk { // Start of the library

#ifdef DEREKS_RESTRICTED_LIB_QUANTUM_H // Aliases for quantum.h

    template<typename T> requires std::copyable<T>
    using ternary = drk::ternary_quantum<T>;

    template<typename Own, typename Linked>
    using linked = drk::linked_quantum<Own, Linked>;

    template<typename T, class Strictness = drk::quantum::free_uncertain_trait>
    using uncertain = drk::uncertain_quantum<T, Strictness>;

    using qstate = drk::quantum_state;
    using enum drk::quantum_state;

#endif // End of Aliases for quantum.h

#ifdef DEREKS_RESTRICTED_LIB_HIVE_H // Aliases for hive.h

    using namespace drk::bee;

#endif // End of Aliases for hive.h

} // End of the library

#endif // End of DEREKS_RESTRICTED_LIB_ALIASES_H