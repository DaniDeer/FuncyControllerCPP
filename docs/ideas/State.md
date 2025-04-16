# State pattern

The State monad is used to model stateful computations. This pattern allows you to pass and modify state throughout computations in a functional way.

## Use Cases

Managing state in a pure functional way without mutating variables directly, such as modeling a game world or handling complex transformations.

## Class template ideation

This can help manage state transitions in a functional way, where each function call returns a new state and value.

```cpp
template <typename S, typename A>
class State {
public:
    using Func = std::function<std::pair<A, S>(S)>;

    State(Func func) : runFunc(func) {}

    std::pair<A, S> run(S state) const {
        return runFunc(state);
    }

    State<S, B> map(B (*f)(A)) const {
        return State<S, B>([=](S state) {
            auto [result, newState] = runFunc(state);
            return std::make_pair(f(result), newState);
        });
    }

    State<S, A> flatMap(std::function<State<S, A>(A)> f) const {
        return State<S, A>([=](S state) {
            auto [result, newState] = runFunc(state);
            return f(result).run(newState);
        });
    }

private:
    Func runFunc;
};
```

## Example

```cpp
int main() {
    // State manipulation example
    State<int, int> addOne([](int state) {
        return std::make_pair(state + 1, state + 1);
    });

    auto [result, newState] = addOne.run(5);

    // Test: Should print 6 (new state)
    std::cout << "State Test Result: " << result << std::endl;
    return 0;
}
```
