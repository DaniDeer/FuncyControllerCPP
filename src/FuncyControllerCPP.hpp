#ifndef FUNCYCONTROLLERCPP_MAIN_HPP 
#define FUNCYCONTROLLERCPP_MAIN_HPP 

// Core Monads/Types
#include "Maybe.hpp"
#include "Either.hpp"
#include "IO.hpp"
#include "Async.hpp"

// Core Helpers (depend on IO/Either)
#include "FunctionalHelpers.hpp"

// --- Namespace Alias (Optional Convenience) ---
// namespace fp = functional_cpp; // Uncomment if you add a namespace

// Note: Platform-specific factories are not included by default.
// User should include "AsyncFactories.hpp" manually if needed, e.g.
// #include <AsyncFactories.hpp> // If FunctionalCPP is in Arduino libraries path

#endif // FUNCYCONTROLLERCPP_MAIN_HPP 