// ==================== IO<T> and IO<void> ====================
#ifndef FUNCYCONTROLLERCPP_IO_HPP
#define FUNCYCONTROLLERCPP_IO_HPP

#include <functional>   // For std::function
#include <utility>      // For std::move
#include <type_traits>  // For std::invoke_result_t, decltype
#include <Arduino.h>    // For String in toString

namespace funcy_controller_cpp {

// Forward Declarations. Important for issues with order definitions!
template<typename T> class IO;
template<> class IO<void>; // Important: Declare the specialization

// ==================== IO<T> ====================
template<typename T>
class IO {
public:
  using value_type = T; // Allows deduction in generic flatMap from IO<void>
  using Func = std::function<T()>;  // The type of the side-effecting computation

  // Constructor taking the effectful function
  explicit IO(Func func) : effect(func) {}

  // Execute the side effect and return the result
  T run() const {
    return effect();
  }

  // map: Apply a synchronous function (T -> U) after the effect runs
  // F should be callable with T and return U
  template<typename F, typename U = decltype(std::declval<F>()(std::declval<T>()))>
  IO<U> map(F f) const {
    return IO<U>([=]() { return f(effect()); });
  }

  // flatMap (bind): Chain with a function that returns another IO (T -> IO<U>)
  // F should be callable with T and return IO<U>
  template<typename F, typename IO_U = decltype(std::declval<F>()(std::declval<T>()))>
  IO<typename IO_U::value_type> flatMap(F f) const {
    return IO<typename IO_U::value_type>([=]() {
      return f(effect()).run();
    });
  }

  
  // then: Run this IO<T>, discard T, run next_io, return U
  template<typename NextIO> // NextIO should be some IO<U>
  IO<typename NextIO::value_type> then(const NextIO& next_io) const {
    using U = typename NextIO::value_type;
    return IO<U>([effect = this->effect, next_io]() { // Capture by value
      effect(); // Run original effect, discard result T
      return next_io.run(); // Run next IO and return its result U
    });
  }

  // thenKeep: Run this IO<T>, run next_void_io, return original T
  // Note: Using IO<void> directly for VoidIO type for simplicity here.
  // This is why forward declarations are important!
  IO<T> thenKeep(const IO<void>& next_void_io) const { // Takes const reference
    return IO<T>([effect = this->effect, &next_void_io]() { // Capture next_void_io by reference (&)
      T result = effect();    // Run original effect, store result T
      next_void_io.run();     // Run the void effect via the reference
      return result;          // Return original result T
    });
  }

  String toString() const {
    //return "IO<" + String(typeid(T).name()) + "> operation";
    return "IO<T> operation"; // RTTI-disabled friendly for uControllers
  }

private:
  std::function<T()> effect;
};
// ==================== IO<void> Specialization ====================
template<>
class IO<void> {
public:
  using value_type = void;
  using Func = std::function<void()>; // The computation with side effects

  explicit IO(Func func) : effect(func) {}

  // Run the IO<void> effect
  void run() const {
    effect();
  }

  // map: execute side effect, then another side effect
  IO<void> map(std::function<void()> f) const {
    return IO<void>([=]() {
      effect();
      f();
    });
  }

  // flatMap: execute side effect, then chain another IO<void>
  IO<void> flatMap(std::function<IO<void>()> f) const {
    return IO<void>([=]() {
      effect();
      f().run();
    });
  }

  // flatMap: execute side effect, then chain IO
  template<typename F, typename IO_U = std::invoke_result_t<F>>
  auto flatMap(F f) const -> IO<typename IO_U::value_type> {
    return IO<typename IO_U::value_type>([=]() {
      effect();
      return f().run();
    });
  }

  // then: Run this IO<void>, run next_io, return U
  template<typename NextIO> // NextIO should be some IO<U>
  IO<typename NextIO::value_type> then(const NextIO& next_io) const {
    using U = typename NextIO::value_type;
    return IO<U>([effect = this->effect, next_io]() { // Capture by value
      effect(); // Run void effect
      return next_io.run(); // Run next IO and return its result U
    });
  }

  String toString() const {
    return "IO<void> operation";
  }

private:
  std::function<void()> effect;
};

} // namespace funcy_controller_cpp

#endif // FUNCYCONTROLLERCPP_IO_HPP