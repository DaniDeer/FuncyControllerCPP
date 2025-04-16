# Reader pattern

The Reader pattern is used to model computations that depend on an external environment or configuration. It's often used for dependency injection, where the environment provides data that functions need to operate on.

## Use Cases

Access configuration data (like database settings or user credentials) in various parts of your application.

## Class template ideation

You can use Reader to inject an environment into functions and chain computations based on it.

```cpp
template<typename R, typename A>
class Reader {
public:
    using Func = std::function<A(R)>;

    Reader(Func func) : runFunc(func) {}

    A run(R env) const {
        return runFunc(env);
    }

    Reader<R, B> map(B (*f)(A)) const {
        return Reader<R, B>([=](R env) {
            return f(runFunc(env));
        });
    }

private:
    Func runFunc;
};
```

## Example

```cpp
int main() {
    // Define a configuration structure
    struct Config {
        int value;
    };

    // Create a Reader that uses the configuration
    Reader<Config, int> reader([](Config config) {
        return config.value;
    });

    // Run the Reader with a configuration
    Config config{42};
    int result = reader.run(config);

    // Test: Should print 42
    std::cout << "Reader Test Result: " << result << std::endl;
    return 0;
}
```
