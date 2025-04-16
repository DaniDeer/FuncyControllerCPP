# Option pattern

The Option monad is similar to Maybe but typically used in a more general sense to represent optional values, which may or may not be present.

## Use Cases

This pattern is useful for handling cases where you might have a value or no value (e.g., a result that may not exist in a database lookup).

## Class template ideation

This pattern can be used to handle cases where an optional value is returned, without dealing with nulls or exceptions.

```cpp
template <typename T>
class Option {
public:
    static Option<T> Some(T value) {
        return Option<T>(value, true);
    }

    static Option<T> None() {
        return Option<T>(T(), false);
    }

    bool isSome() const {
        return hasValue;
    }

    bool isNone() const {
        return !hasValue;
    }

    T unwrap() const {
        if (!hasValue) {
            throw std::runtime_error("No value present");
        }
        return value;
    }

private:
    Option(T value, bool hasValue) : value(value), hasValue(hasValue) {}

    T value;
    bool hasValue;
};
```

## Example

```cpp
int main() {
    Option<int> someOption = Option<int>::Some(42);
    Option<int> noneOption = Option<int>::None();

    // Test: Should print 42 for Some and throw for None
    if (someOption.isSome()) {
        std::cout << "Option Some: " << someOption.unwrap() << std::endl;
    }

    try {
        std::cout << "Option None: " << noneOption.unwrap() << std::endl; // Should throw exception
    } catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }
    return 0;
}
```
