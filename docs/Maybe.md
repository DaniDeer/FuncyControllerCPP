# The Maybe<T> Pattern

The `Maybe<T>` pattern is used to represent the presence or absence of a value. The idea is to model operations that may or may not yield a value, without throwing exceptions or dealing with null. This might be useful in cases like retrieving an item from a collection that might not contain it or parsing a string into a number or a complex object where the result might be invalid. It's a way to handle computations that might fail to return a value or might result in null or undefined in other paradigms, without the need for explicit error handling or null checks.

## Concept

1. **Nothing** -> representing the absence of a value
2. **Just** -> representing the presence of a value

## Benefits

1. **Optional return types**: If you have a function that might fail to compute a value, instead of returning null or a special value, you can return **Nothing**.
2. **Chaining operations**: If you are working with a value that may or may not exist, you can chain operations without worrying about null pointer exceptions.
3. **Eliminating error-prone null checks**: By explicitly wrapping the possibility of null inside the Maybe type, you eliminate explicit null checks in your code.

## Example

Simple examples:

```cpp
Maybe<int> findFirstEven(const std::vector<int>& numbers, size_t index = 0) {
  // Base case: if we've exhausted the list, return Nothing
  if (index >= numbers.size()) {
    return Maybe<int>::Nothing();
  }

  // Recursive case: check the current element
  if (numbers[index] % 2 == 0) {
    return Maybe<int>::Just(numbers[index]);
  }

  // Recur for the next element
  return findFirstEven(numbers, index + 1);
}
```

Chained example:

```cpp
Maybe<int> readSensor(bool ok) {
  return ok ? Maybe<int>(analogRead(A0)) : Maybe<int>();
}

Maybe<float> toVoltage(int raw) {
  if (raw < 10) return {}; // bad reading
  return Maybe<float>(raw * 3.3 / 1023.0);
}

Maybe<float> toTemperature(float volts) {
  return Maybe<float>((volts - 0.5) * 100); // some formula
}

void loop() {
  auto result = readSensor(true)
    .flatMap(toVoltage)
    .flatMap(toTemperature);

  if (result.isJust()) {
    Serial.print("Temp: ");
    Serial.println(result.unwrap());
  } else {
    Serial.println("Failed to read temp");
  }

  delay(1000);
}
```

**What did we gain?**

- No `if/else` chains or error flags
- Each function only foucses on _one thing_
- Easy to test in isolation
- You can even log or inject debugging logic between steps

## Implementation

### Template Class API

- **`Just` and `Nothing`**: Static methods to create instances of Maybe. `Just` wraps a value, and `Nothing` represents the absence of a value.
- **Introspection**: Methods `isJunst()` and `isNothing` allow to ckeck if the `Maybe` contains a value or not.
- **`map()`**: transforms the value inside the `Maybe` (if present) using a given function.
- **`flatMap()`**: similar to map(), but the function passed in returns another `Maybe<T>` of same type `T`, allowing to chain computations that may file
- **`match()`**: method for pattern matching, where both `Just` or ``Nothing` are handled.
- **`fold()`**: similar to `match()`, but combines the results from both cases into a single value.
