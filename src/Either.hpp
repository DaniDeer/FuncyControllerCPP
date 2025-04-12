// ==================== Either<T, E> ====================
#ifndef FUNCYCONTROLLERCPP_EITHER_HPP 
#define FUNCYCONTROLLERCPP_EITHER_HPP 

#include <functional>   // For potential use in lambdas passed to methods
#include <Arduino.h>    // For String type used in toString()

namespace funcy_controller_cpp {

template<typename T, typename E>
class Either {
public:
  // let the compiler access IO_Either_U::value_type::value_type
  using value_type = T;
  using error_type = E;
  
  // Constructors
  static Either Right(T value) {
    return Either(value, {}, true);
  }

  static Either Left(E error) {
    return Either({}, error, false);
  }

  // Introspection
  bool isRight() const { return right; }
  bool isLeft() const { return !right; }

  // Accessors (Use with caution, prefer match/fold)
  // Hint: In FP the best way is to access the func
  // outputs via match and fold.
  T unwrapRight() const { return value; }
  E unwrapLeft() const { return error; }

  // Map: transform Right
  template<typename F>
  auto map(F f) const {
    using R = decltype(f(value));
    if (right) return Either<R, E>::Right(f(value));
    return Either<R, E>::Left(error);
  }

  // flatMap (bind): transform Right into another Either
  template<typename F>
  auto flatMap(F f) const {
    using Ret = decltype(f(value));
    if (right) return f(value);
    return Ret::Left(error);
  }

  // mapLeft: transform Left (the error)
  template<typename F>
  auto mapLeft(F f) const {
    using E2 = decltype(f(error));
    if (right) return Either<T, E2>::Right(value);
    return Either<T, E2>::Left(f(error));
  }

  // match: pattern-match both sides
  // handle both Rigth and Error cases seperately
  template<typename LeftFn, typename RightFn>
  auto match(LeftFn onLeft, RightFn onRight) const {
    return right ? onRight(value) : onLeft(error);
  }

  // fold: convert to a value
  // combines the results from both cases (right OR left) into a single value
  template<typename LeftFn, typename RightFn>
  auto fold(LeftFn onLeft, RightFn onRight) const {
    return right ? onRight(value) : onLeft(error);
  }

  // Debug toString
  String toString() const {
    return right ? "Right(" + value + ")" : "Left(" + error + ")";
  }

private:
  T value;
  E error;
  bool right;

  // Default construct T and E if possible, or handle initialization carefully
  Either(T val, E err, bool isRight)
    : value(val), error(err), right(isRight) {}
  
  // Allow default construction if T and E are default constructible
  Either() : right(false) {} // Default to Left? Or make private constructor mandatory? Needs thought.

};

} // namespace funcy_controller_cpp

#endif // FUNCYCONTROLLERCPP_EITHER_HPP