#ifndef FUNCYCONTROLLERCPP_FUNCTIONALHELPERS_HPP
#define FUNCYCONTROLLERCPP_FUNCTIONALHELPERS_HPP

#include "IO.hpp"          // Needs IO definition
#include "Either.hpp"      // Needs Either definition
#include <functional>   // For std::function
#include <type_traits>  // For decltype, type deduction helpers

namespace funcy_controller_cpp {

// ==================== IO Helpers ====================

// pure: lift a value to IO<T>
template<typename T>
IO<T> pure(const T& value) {
  return IO<T>([value]() { return value; });
}

// unit: return an IO<void> with no side-effect
inline IO<void> unit() {
  return IO<void>([]() {});
}

// ============ IO / Either Composition Helpers ============

// liftIO: convert IO<T> to IO<Either<T, E>> (always Right)
template<typename T, typename E>
IO<Either<T, E>> liftIO(IO<T> io) {
  return IO<Either<T, E>>([io]() {
    return Either<T, E>::Right(io.run());
  });
}

// liftIOLeft: convert IO<E> to IO<Either<T, E>> (always Left)
template<typename T, typename E>
IO<Either<T, E>> liftIOLeft(IO<E> io) {
  return IO<Either<T, E>>([io]() {
    return Either<T, E>::Left(io.run());
  });
}

//liftVoidIO() - Convert IO<void> to IO<Either<T, E>> (always Right with default T)
template<typename T, typename E>
IO<Either<T, E>> liftVoidIO(IO<void> io) {
  return IO<Either<T, E>>([io]() {
    io.run();
    return Either<T, E>::Right(T{}); // Return Right with default T
  });
}

// liftIOtoEither(): Wrap an IO<T> into IO<Either<E, T>> based on predicates
// Takes an IO<T>, an error function (T -> E), and an error predicate (T -> bool)
// Applying some error-checking logic
template<typename T, typename E>
IO<Either<E, T>> liftIOtoEither(
  IO<T> io,
  std::function<E(T)> errorFn,
  std::function<bool(T)> isError
  ) {
  return IO<Either<E, T>>([=]() {
    T val = io.run();
    if (isError(val)) {
      return Either<E, T>::Left(errorFn(val));
    } else {
      return Either<E, T>::Right(val);
    }
  });
}

// flatMapIOEither (TaskEither): Unwrap the IO, Inspect the Either,
//  If it's a Right, call f() (which returns another IO<Either>),
//  If it's a Left, short-circuit and return that inside a new IO.
// Chains IO<Either> ops
template<typename T, typename E, typename F, typename IO_Either_U = decltype(std::declval<F>()(std::declval<T>()))>
IO<Either<typename IO_Either_U::value_type::value_type, E>> flatMapIOEither(
  IO<Either<T, E>> ioEither,
  F f // T -> IO<Either<U, E>>
  ) {
  using U = typename IO_Either_U::value_type::value_type;
  return IO<Either<U, E>>([=]() {
    Either<T, E> result = ioEither.run();
    if (result.isRight()) {
      return f(result.unwrapRight()).run();
    } else {
      return Either<U, E>::Left(result.unwrapLeft());
    }
  });
}

// mapIOEither(): Transform the Right value inside IO<Either<T, E>> using (T -> U)
template<typename T, typename E, typename F>
auto mapIOEither(IO<Either<T, E>> io, F f) {
  using U = decltype(f(std::declval<T>()));
  return IO<Either<U, E>>([=]() {
    return io.run().map(f);  // Only transforms Right
  });
}

// mapLeftIOEither(): Transform the Left value inside IO<Either<T, E>> using (E -> E2)
// This is useful for error handling, where you want to transform the error type
template<typename T, typename E, typename F>
auto mapLeftIOEither(IO<Either<T, E>> io, F f) {
  using E2 = decltype(f(std::declval<E>()));
  return IO<Either<T, E2>>([=]() {
    return io.run().mapLeft(f);  // Only transforms Left
  });
}

// Add other helpers as needed

} // namespace funcy_controller_cpp

#endif // FUNCYCONTROLLERCPP_FUNCTIONALHELPERS_HPP