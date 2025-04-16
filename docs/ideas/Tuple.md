# Tuple (or Pair) pattern

A Tuple (or Pair) is a simple structure that holds two or more related values, often used for returning multiple results from a function.

## Use Cases

If you need to return more than one value from a function (e.g., a value and its associated metadata), a tuple can model this.

## Class template ideation

This allows you to combine multiple values in a single structure and work with them as a unit.

```cpp
template<typename A, typename B>
class Tuple {
public:
    Tuple(A first, B second) : first(first), second(second) {}

    A getFirst() const {
        return first;
    }

    B getSecond() const {
        return second;
    }

private:
    A first;
    B second;
};
```

## Example

```cpp
int main() {
    // Tuple test
    Tuple<int, std::string> tuple(1, "Hello");

    // Test: Should print 1 and "Hello"
    std::cout << "Tuple Test Result: " << tuple.getFirst() << ", " << tuple.getSecond() << std::endl;
    return 0;
}
```