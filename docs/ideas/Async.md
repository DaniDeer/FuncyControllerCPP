# Async pattern

The Async pattern is used for handling asynchronous computations in a functional way. It allows you to represent computations that are delayed or computed in parallel.

## Use Cases

If you're dealing with async operations, such as network requests, file I/O, or background tasks, the Async pattern can help you compose these operations functionally.

## Class template ideation

You can use the Async monad to compose asynchronous operations while keeping them pure and easy to reason about.

```cpp
template <typename T>
class Async {
public:
    using Func = std::function<std::future<T>()>;

    Async(Func func) : runFunc(func) {}

    std::future<T> run() const {
        return runFunc();
    }

    Async<T> map(std::function<T(T)> f) const {
        return Async<T>([=]() {
            return std::async([=]() {
                return f(runFunc().get());
            });
        });
    }

private:
    Func runFunc;
};
```

## Example

```cpp
int main() {
    // Async example
    Async<int> async([]() {
        return std::async([]() {
            return 42;
        });
    });

    auto futureResult = async.run();
    int result = futureResult.get();

    // Test: Should print 42
    std::cout << "Async Test Result: " << result << std::endl;
    return 0;
}
```