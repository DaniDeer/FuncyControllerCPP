// ==================== Async<T> ====================
#ifndef FUNCYCONTROLLERCPP_ASYNC_HPP
#define FUNCYCONTROLLERCPP_ASYNC_HPP

#include <functional>   // For std::function
#include <utility>      // For std::move
#include <type_traits>  // For std::enable_if_t, std::is_same, decltype, etc.

namespace funcy_controller_cpp {

// Forward declaration for Async<void> specialization within the namespace
template<typename T> class Async;
template<> class Async<void>;

// ==================== Async<T> ====================
template<typename T>
class Async {
public:
  using value_type = T;
  // The type of the callback function that receives the result
  using Callback = std::function<void(T)>;
  // The underlying computation: takes a callback and sets up the async operation
  // It's responsible for eventually calling the callback with the result.
  using Computation = std::function<void(Callback)>;

  // Construct an Async from a computation function
  // Made explicit to prevent accidental conversions
  explicit Async(Computation computation) : computation_(std::move(computation)) {}

  // --- Core Methods ---

  // "Run" the asynchronous operation.
  // Provide the callback that will eventually receive the result.
  void runAsync(Callback onComplete) const {
      // Execute the stored computation, passing it the final callback
      computation_(std::move(onComplete));
  }

  // map: Transform the future result T -> U using function f
  // F should be callable with T and return U
  template<typename F>
  auto map(F f) const {
      using U = decltype(f(std::declval<T>())); // Deduce return type U

      // Return a *new* Async<U>
      return Async<U>([this_computation = computation_, f_moved = std::move(f)](std::function<void(U)> callback_u) {
          // When the new Async<U> is run (with callback_u)...
          // ...run the original computation (this_computation)...
          this_computation([f_inner = std::move(f_moved), callback_u_inner = std::move(callback_u)](T result_t) {
              // ...when the original computation completes (calling this lambda)...
              // ...transform the result T -> U...
              U result_u = f_inner(result_t);
              // ...and call the final callback (callback_u) with the transformed result.
              callback_u_inner(result_u);
          });
      });
  }

  // flatMap (bind): Chain asynchronous operations.
  // F should be callable with T and return an Async<U>
  // --- flatMap Overload 1: When f returns Async<U> where U is NOT void ---
  template <typename F,
            // Deduce types related to the function f's return value
            typename AsyncU = decltype(std::declval<F>()(std::declval<T>())),
            typename U = typename AsyncU::value_type>
  auto flatMap(F f) const
    // SFINAE: Enable this overload only if U is NOT void
    -> std::enable_if_t<!std::is_same<U, void>::value, Async<U>>
  {
    // We know U is not void here.
    return Async<U>([this_computation = computation_, f_moved = std::move(f)](std::function<void(U)> final_callback_u) {
      // Original computation takes a callback that receives T
      this_computation([f_inner = std::move(f_moved), final_callback_u_inner = std::move(final_callback_u)](T result_t) {
        // When original completes, run f to get the next Async<U>
        AsyncU next_async = f_inner(result_t);
        // Run the next async operation, passing the final callback (type: std::function<void(U)>)
        next_async.runAsync(final_callback_u_inner);
      });
    });
  }

  // --- flatMap Overload 2: When f returns Async<void> ---
  template <typename F,
            // Deduce types related to the function f's return value
            typename AsyncU = decltype(std::declval<F>()(std::declval<T>())),
            typename U = typename AsyncU::value_type>
  auto flatMap(F f) const
    // SFINAE: Enable this overload only if U IS void
    -> std::enable_if_t<std::is_same<U, void>::value, Async<void>> // Explicitly return Async<void>
  {
    // We know U is void here, so AsyncU is Async<void>.
    // The final callback type needed is std::function<void()>
    return Async<void>([this_computation = computation_, f_moved = std::move(f)](std::function<void()> final_callback) {
      // Original computation takes a callback that receives T
      this_computation([f_inner = std::move(f_moved), final_callback_inner = std::move(final_callback)](T result_t) {
        // When original completes, run f to get the next Async<void>
        AsyncU next_async = f_inner(result_t); // next_async is Async<void> here
        // Run the next async operation, passing the final callback (type: std::function<void()>)
        next_async.runAsync(final_callback_inner);
      });
    });
  }

  // --- Static Factory Methods ---

  // pure: Create an Async operation that completes immediately with a known value.
  static Async<T> pure(T value) {
      return Async<T>([val = std::move(value)](Callback cb) {
          // Immediately call the callback with the value
          cb(val);
      });
  }

  // Factory to create an Async from a function that takes a callback
  // Useful for wrapping existing callback-based APIs directly
  static Async<T> create(Computation computation) {
        return Async<T>(std::move(computation));
  }


private:
  Computation computation_; // The stored description of the async operation
};

// ==================== Async<void> Specialization ====================
template<>
class Async<void> {
public:
    using value_type = void;
    using Callback = std::function<void()>;
    using Computation = std::function<void(Callback)>;

    explicit Async(Computation computation) : computation_(std::move(computation)) {}

    void runAsync(Callback onComplete) const {
        computation_(std::move(onComplete));
    }

    // map: Perform an additional synchronous void action after this Async completes.
    // Note: Map for Async<void> usually means sequencing synchronous void actions,
    // flatMap is used for sequencing asynchronous actions.
    Async<void> map(std::function<void()> action) const {
        return Async<void>([this_computation = computation_, action_moved = std::move(action)](Callback final_callback) {
            this_computation([action_inner = std::move(action_moved), final_callback_inner = std::move(final_callback)]() {
                // When original completes, run the action, then the final callback
                action_inner();
                final_callback_inner();
            });
        });
    }


    // flatMap: Sequence another Async operation after this one completes.
    // F must be callable and return an Async<U>.
    template <typename F>
    auto flatMap(F f) const {
        // Deduce the nested Async<U> type returned by f
        using AsyncU = decltype(f()); // f takes no arguments here
        // Deduce the value type U from the nested Async<U>
        using U = typename AsyncU::value_type;

        return Async<U>([this_computation = computation_, f_moved = std::move(f)](std::function<void(U)> final_callback_u) {
            this_computation([f_inner = std::move(f_moved), final_callback_u_inner = std::move(final_callback_u)]() {
                 // When original completes, call f() to get the next Async<U>
                 AsyncU next_async = f_inner();
                 // Run the next async operation, passing the final callback
                 next_async.runAsync(final_callback_u_inner);
            });
        });
    }

    // Special flatMap for chaining Async<void> -> Async<void>
    Async<void> flatMap(std::function<Async<void>()> f) const {
         return Async<void>([this_computation = computation_, f_moved = std::move(f)](Callback final_callback) {
             this_computation([f_inner = std::move(f_moved), final_callback_inner = std::move(final_callback)]() {
                 Async<void> next_async = f_inner();
                 next_async.runAsync(final_callback_inner);
             });
         });
    }

    // --- Static Factory Methods ---

    // unit: Create an Async<void> that completes immediately.
    static Async<void> unit() {
        return Async<void>([](Callback cb){
            cb();
        });
    }

     // Factory to create an Async from a function that takes a callback
    static Async<void> create(Computation computation) {
         return Async<void>(std::move(computation));
    }

private:
    Computation computation_;
};

// ==================== Async<Either<T, E>> Specialization ====================
// ... to be implemented later

} // namespace funcy_controller_cpp

#endif // FUNCYCONTROLLERCPP_ASYNC_HPP