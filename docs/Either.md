# The Either<T, E> Pattern

The `Either<T, E>` pattern allows you to **carry either a successful result (Right) or an error (Left)**.
This is useful for propagating errors with context instead of just returning a default or failing silently.

## Concept

1. **Right T** -> representing success and returns the value of type T on the Right side
2. **Left E** -> representing failure and returns the value of type E on the Left side

## Benefits

- Clear, testable error handling
- No hidden global flags or state
- Supports composition: every step either succeeds or stops early
- Cleaner than exceptions

## Example

Sensor readings with errors:

```cpp
Either<int, String> readRawSensor() {
  int raw = analogRead(A0);
  if (raw < 10) return Either<int, String>::Left("Sensor read too low");
  return Either<int, String>::Right(raw);
}

Either<float, String> rawToVoltage(int raw) {
  float v = raw * 3.3 / 1023.0;
  if (v > 3.0) return Either<float, String>::Left("Voltage too high");
  return Either<float, String>::Right(v);
}

Either<float, String> voltageToTemp(float v) {
  float temp = (v - 0.5) * 100;
  if (temp < -40 || temp > 125) return Either<float, String>::Left("Temp out of range");
  return Either<float, String>::Right(temp);
}
```

Chaining with `flatMap()` and `map()`:

```cpp
void loop() {
  auto result = readRawSensor()
    .flatMap(rawToVoltage)
    .flatMap(voltageToTemp)
    .map([](float temp) {
      Serial.print("Temp: ");
      Serial.println(temp);
      return true;
    });

  if (result.isLeft()) {
    Serial.print("Error: ");
    Serial.println(result.unwrapLeft());
  }

  delay(1000);
}
```

> Instead of using `unwrapLeft()` use the `match()` or `fold()` method to access values and handle errors.
> ToDo: Rewrite the example to match().

### Bonus: Alias Types

```cpp
using Result<T> = Either<T, String>;
```

Now you can return `Result<int>`, `Result<float>`, etc., without repeating `Either<T, String>` everywhere.

## Using Maybe<T> or Either<T, E>?

### Benefits of Either<T, E> over Maybe<T>

- **Error traceability**: Know why something failed
- **No silent failures**: Avoid confusion over missing values
- **Better debugging/logging**: Can show detailed messages
- **Composable**: Each function only needs to handle success/failure locally
- **Safer than exceptions**: No runtime throw/catch overhead, deterministic flow
- **Functional style**: Promotes chaining, immutability, and clarity

### Benefits of Maybe<T> over Either<T, E>

- **Simplicity**: Lightweight and minimal - just "value" or "nothing"
- **No error modeling**: You don´t need to define or think about error types
- **Memory footprint**: Smaller no need to store error values - great for uControllers
- **API clarity**: Makes it obvious: _absence of a value is expected_ (not an error)!
- **Less boilerplate**: No need to write Either<T, String> or provide error descriptions.
- **Performance**: Fewer branches or storage = faster and smaller binary

### Summary

- Maybe = simpler, lighter, ideal for optional values.
- Either = richer, better for error handling with explanations.

| **Use Maybe<T> when...**                      | **Use Either<T, E> when...**                                |
| --------------------------------------------- | ----------------------------------------------------------- |
| Simplicity is preferred                       | You want detailed error messages                            |
| You don’t need to explain failure             | You need rich error reporting/logging                       |
| Space and performance are critical            | You can afford a bit more complexity                        |
| “Nothing” is a valid and expected outcome     | Errors are exceptional and meaningful                       |
| You want to avoid boilerplate or error typing | You’re composing complex pipelines with many failure points |
| _"this didn't work, move on"_                 | _"this didn´t work, hold on and here is why"_               |
