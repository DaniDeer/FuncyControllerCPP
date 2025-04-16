# Writer pattern

The Writer monad is used for accumulating logs or outputs alongside computations. It’s typically used for functions that produce side effects in the form of logs or other outputs.

## Use Cases

When you need to maintain an accumulator (like a log) while performing computations, the Writer pattern lets you accumulate logs alongside the result.

## Class template ideation

This could be useful for logging transformations or maintaining an audit trail of computations.

```cpp
template <typename W, typename A>
class Writer {
public:
    using Func = std::function<std::pair<A, W>()>;

    Writer(Func func) : runFunc(func) {}

    std::pair<A, W> run() const {
        return runFunc();
    }

    Writer<W, B> map(B (*f)(A)) const {
        return Writer<W, B>([=]() {
            auto [result, log] = runFunc();
            return std::make_pair(f(result), log);
        });
    }

private:
    Func runFunc;
};
```

## Example

```cpp
int main() {
    // Writer example with log accumulation
    Writer<std::string, int> writer([]() {
        return std::make_pair(10, "Processing complete");
    });

    auto [result, log] = writer.run();

    // Test: Should print 10 and the log message
    std::cout << "Writer Test Result: " << result << ", Log: " << log << std::endl;
    return 0;
}
```
