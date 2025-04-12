// ==================== Maybe<T> ====================
// Concept:
//  - Nothing (representing the absence of a value).
//  - Just (representing the presence of a value).
//  - operations that may or may not yield a value, without throwing exceptions or dealing with null
// Use cases:
//  - Retrieving an item from a collection that might not contain it.
//  - Parsing a string into a number or a complex object where the result might be invalid.

#ifndef FUNCYCONTROLLERCPP_MAYBE_HPP 
#define FUNCYCONTROLLERCPP_MAYBE_HPP 

#include <functional>   // For potential use in lambdas passed to methods
#include <Arduino.h>    // For String type used in toString()

namespace funcy_controller_cpp {

template<typename T>
class Maybe {
public:
  // Constructors
  static Maybe Just(T value) {
      return Maybe(value, true);
  }

  static Maybe Nothing() {
      return Maybe(T(), false);
  }

  // Introspection
  bool isJust() const { return hasValue; }
  bool isNothing() const { return !hasValue; }

  // Accessor
  // WARNING: Unwrapping Nothing is unsafe. Prefer match() or fold().
  // Hint: In FP the best way is to access the func
  // outputs via match and fold.
  

  // Map: transform Just value
  template<typename F>
  auto map(F f) const {
    // Deduce return type R from function f applied to value
    using R = decltype(f(value));
    if (hasValue) return Maybe<R>::Just(f(value));
    return Maybe<R>::Nothing();
  }

  // FlatMap (bind): transform Just value into another Maybe
  template<typename F>
  auto flatMap(F f) const {
    // Deduce the return type (which must be a Maybe<...>)
    using Ret = decltype(f(value));
    if (hasValue) return f(value);
    return Ret::Nothing(); // Return Nothing of the same Maybe<...> type
  }

  // Accessor -> outputs via match and fold.
  // Hint: In FP the best way is to access the func
  // WARNING: Unwrapping Nothing is unsafe. Prefer match() or fold().
  // Therefore unwrap() is not implemented!

  // Match: pattern-match both sides, 
  // handle both Just and Nothing cases separately
  template<typename JustFn, typename NothingFn>
  auto match(JustFn onJust, NothingFn onNothing) const {
    // Result type is deduced from the return types of onJust and onNothing
    // Note: They should ideally return the same type or compatible types.
    return hasValue ? onJust(value) : onNothing();
  }

  // Fold: convert to a value (default if Nothing)
  // combines the results from both cases (Just OR Nothing) into a single value
  template<typename JustFn, typename NothingFn>
  auto fold(JustFn onJust, NothingFn onNothing) const {
    // Result type is deduced from the return types of onJust and onNothing
    // Note: They must return the same type or compatible types.
    return hasValue ? onJust(value) : onNothing();
  }

  // Debug toString
  String toString() const {
    // Note: Requires T to be convertible to String, or have operator<< overloaded
    // if used with a stream-based String implementation. Standard Arduino String
    // constructor often handles primitive types.
    return hasValue ? "Just(" + value + ")" : "Nothing";
  }

private:
  T value;
  bool hasValue;

  Maybe(T val, bool isJust)
    : value(val), hasValue(isJust) {}
};

} // <<< END NAMESPACE

#endif // FUNCYCONTROLLERCPP_MAYBE_HPP