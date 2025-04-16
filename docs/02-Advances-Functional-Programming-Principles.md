# Advanced Functional Programming (FP) Principles

Using more advanced functional programming idioms like _functors, monads_ and _state machines_ can be _approximated_ in C++ even within the Arduino ecosystem and on constrained devices like the ESP8266.

> _Mimicking_ thoes idioms is the goal of this library. I am absolutely aware that the C++ ecosystem and compilers do not offer this as elegantly as native functional languages such as Haskell! But I'm just curious to see how far I can go. And AI as programming buddy nowadays made me do it!

Let´s look at the three idioms from above and see how it all started:

## 1. Functor Pattern (map())

A _functor_ is something you can map a function over. In C++ this usually means templated containers or std::optional, std::vector, etc. But let's do a minimal embedded-friendly version.

`Maybe<T>` and `map()`
```cpp
template<typename T>
class Maybe {
public:
  Maybe() : hasValue(false) {}
  Maybe(T val) : value(val), hasValue(true) {}

  template<typename F>
  auto map(F f) -> Maybe<decltype(f(value))> {
    if (hasValue) return Maybe<decltype(f(value))>(f(value));
    return {};
  }

  bool isJust() const { return hasValue; }
  T unwrap() const { return value; }

private:
  T value;
  bool hasValue;
};
```

Usage:
```cpp
Maybe<int> readSensorValue(bool connected) {
  return connected ? Maybe<int>(analogRead(A0)) : Maybe<int>();
}

void loop() {
  auto maybeVoltage = readSensorValue(true)
    .map([](int raw) { return raw * 3.3 / 1023.0; });

  if (maybeVoltage.isJust()) {
    Serial.println(maybeVoltage.unwrap());
  } else {
    Serial.println("Sensor not connected");
  }

  delay(1000);
}
```

## 2. Monads (flatMap())

A monad adds chaining (`>>=` in Haskell). In C++ it’s `flatMap()`.

Extend `Maybe<T>` with `flatMap()`
```cpp
template<typename T>
class Maybe {
public:
  // ... from before

  template<typename F>
  auto flatMap(F f) -> decltype(f(value)) {
    if (hasValue) return f(value);
    return {};
  }
};
```

Usage:
```cpp
auto maybeTemperature = readSensorValue(true)
  .flatMap([](int raw) {
    if (raw < 10) return Maybe<float>(); // Invalid reading
    return Maybe<float>(raw * 0.488);
  });
```
This resembles monadic chaining with early failure handling.


## Either<T, E>

