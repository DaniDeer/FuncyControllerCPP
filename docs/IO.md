# IO Pattern

The `IO` class will represent a computation that performs a side-effect, such as connecting to WiFi or printing to the serial port. It encapsulates the side-effecting operation, which can be `run()` later, keeping the effect management separate from the business logic.

## Concept

- **Mondaic Operations** -> The IO class will support `map()` and `flatMap()` (also known as bind) to allow chaining of side-effecting computations in a functional way.
- `map()` -> transform a result `IO<T>` into a new `IO<X>` where `T` is transformed into the type `X`
- `flatMap()` -> chain a `IO<T>` into another ``IO<T>` of the same type `T`

## Implementation

For the template implementation the lib distinguishes between `IO<void>` and `IO<T>`.

## Example

Wrap `Serial.print()`:

```cpp
IO<void> logMessage(String msg) {
    return IO<void>([=]() {
        Serial.println(msg);
    });
}
```

Wrap WiFi connection handling:

```cpp
IO<void> connectToWiFi() {
  return IO<void>([]() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin("SSID", "PASSWORD");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("Connected to WiFi!");
  });
}
```
