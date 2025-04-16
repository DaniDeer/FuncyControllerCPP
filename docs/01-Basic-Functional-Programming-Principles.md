# Basic Functional Programming (FP) Principles

Programming uControllers like ESP8266 in the Arduino ecosystem is largely imperative and object-oriented. 
You can apply many functional programming principles e.g. from Haskell to make your code cleaner, more modular, and maintainable.

Here are key concepts that you can **use right away in C++ (without using FuncyControllerCPP lib)**:

## 1. Pure Functions (No Side Effects)

A pure function’s output depends only on its input and has no side effects—perfect for logic processing.
Use this kind of function e.g. to transform a sensor data before displaying or logging it. 
These make testing and reuse easier.

```cpp
float fahrenheitToCelsius(float f) {
  return (f - 32.0) * 5.0 / 9.0;
}

float celsiusToKelvin(float c) {
  return c + 273.15;
}

// Composition
float fahrenheitToKelvin(float f) {
  return celsiusToKelvin(fahrenheitToCelsius(f));
}

```

## 2. Immutability (as much as possible)

C++ doesn´t enforce immutability, but you can use `const` to express intent. 
Immutability prevents accidental changes and improves clarity.

```cpp
const int threshold = 50;  // Won't be changed
const float voltageThreshold = 2.5;

bool isTooHot(const int tempCelsius) {
  return tempCelsius > threshold;
}
```

## 3. Lambdas

Are anonymous functions that can capture context.

Basic lambda:
```cpp
auto adder = [](int a, int b) { return a + b; };
int result = adder(2, 3); // yields 5
```

Lambda with capture:
```cpp
int offset = 10;
auto addOffset = [offset](int x) { return x + offset; };
```

- **Lambda Capture**: You can capture variables from the surrounding scope. 
  - `[=]` captures all by value
  - `[&]` captures all by reference
  - `[x]` captures `x` by value
  - `[&x]` captures `x` by reference

> Note: Later in more advanced FP patterns, you can use this captures in Lambdas in `map()`, `flatMap()`, `match()` and `fold()` etc.

See more examples in next section _Higher-Order Functions_.

## 4. Higher-Order Functions

Are functions that take other functions or return functions.
In CPP you can pass functions as arguments using function pointers or lambdas.

Example: Blinking and say _Hello_
```cpp
void repeat(int times, void (*action)()) {
  for (int i = 0; i < times; ++i) {
    action();
  }
}

void blinkLED() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

// Usage
repeat(3, blinkLED);
```

Or with lambdas:
```cpp
repeat(5, [](){ Serial.println("Hello"); });
```

Example: Wrapping a timer
```cpp
// Wrapping a timer ...
void every(unsigned long interval, void (*task)()) {
  static unsigned long last = 0;
  unsigned long now = millis();
  if (now - last >= interval) {
    last = now;
    task();
  }
}

// ... and using it with lambdas
void loop() {
  every(2000, []() {
    Serial.println("Tick...");
  });
}
```

Example: Math functions
```cpp
int applyTwice(int x, int (*f)(int)) {
  return f(f(x));
}

int increment(int x) { return x + 1; }

void loop() {
  int result = applyTwice(3, increment); // yields 5
  Serial.println(result);
}
```

Or with lambdas:
```cpp
int result = applyTwice(3, [](int x) { return x + 1; });
```


## 5. Declarative Style with Small, Composable Functions

Instead of writing one big loop(), break logic into small composable functions.
Each function does one thing. Like in Haskell, composition makes logic clearer.
Each part can be tested or swapped independently.

```cpp
void readAndDisplayTemperature() {
  int raw = analogRead(A0);
  float tempC = analogToCelsius(raw);
  displayTemperature(tempC);
}
```

Process and send sensor data:
```cpp
int readSensor() {
  return analogRead(A0);
}

float toVoltage(int value) {
  return value * 3.3 / 1023.0;
}

String formatMessage(float voltage) {
  return "Voltage: " + String(voltage, 2);
}

void sendMessage(const String& message) {
  Serial.println(message);
}

void loop() {
  int raw = readSensor();
  float v = toVoltage(raw);
  sendMessage(formatMessage(v));
  delay(1000);
}
```

## 6. Pattern Matching (simulated with switch or maps)

While you can’t do real Haskell-style pattern matching (by using real algebraic types) and decomposing values by patterns, you can simulate it in C++ by using enum, switch, or maps.

In Haskell you can do something like:
```haskell
data Mode = Idle | Active | Error
describe Idle = "Nothing happening"
describe Active = "Working!"
```

The equivalent in C++ is:
```cpp
enum Mode { Idle, Active, Error };

String describe(Mode m) {
  switch (m) {
    case Idle: return "Nothing happening";
    case Active: return "Working!";
    case Error: return "Something's wrong";
    default: return "Unknown";
  }
}
```

This is useful in status reporting, UI modes, connection states, etc.

## 7. Avoiding Shared State / Side Effects

Minimize use of globals and side effects, isolate IO, and separate concerns like:

```cpp
// Pure logic
bool shouldActivateRelay(float temperature) {
  return temperature > 25.0;
}

// IO
void applyRelayState(bool activate) {
  digitalWrite(RELAY_PIN, activate ? HIGH : LOW);
}
```

Separating logic from hardware

```cpp
bool isOverThreshold(float temp, float threshold) {
  return temp > threshold;
}

void controlFan(bool turnOn) {
  digitalWrite(D2, turnOn ? HIGH : LOW);
}

void loop() {
  float temp = analogRead(A0) * 3.3 / 1023.0 * 100;
  bool fanState = isOverThreshold(temp, 30.0);
  controlFan(fanState);
}
```

## 8. Lazy Evaluation (simulated via closures or deferred execution)

C++ doesn’t support true lazy evaluation, but you can simulate it by using lambdas or generators. This helps when you want to define a pipeline or conditional read.

```cpp
std::function<int()> lazySensorRead = []() {
  return analogRead(A0);  // Deferred until called
};

// Later in code
int value = lazyReadSensor();  // Not executed until needed
```

## 9. State as Data

Rather than having booleans or flags scattered around your codebase, collect all relevant state in a struct.
This mirrors Haskell’s State monad (kind of), and makes transitions explicit and testable.

Collect all relevant state in a struct:
```cpp
struct AppState {
  bool isConnected;
  float lastVoltage;
  unsigned long lastUpdate;
};
```

Then functions become pure transforms:
```cpp
AppState updateVoltage(AppState state, float newReading) {
  state.lastVoltage = newReading;
  return state;
}
```

You could then build a pipeline:
```cpp
AppState next = updateVoltage(prev, analogRead(A0) * 3.3 / 1023.0);
```

This isolates data from logic and mimics Haskell-style state monads.

Other example: temperature relay
```cpp
struct AppState {
  float temperature;
  bool relayOn;
  bool wifiConnected;
};

AppState updateRelay(AppState state) {
  state.relayOn = state.temperature > 30.0;
  return state;
}
```

## 10. State Machine (Functinal-style)

Instead of if/else with flags, use functions and data to represent states.

State enum and function table
```cpp
enum class State {
  ConnectingWiFi,
  ConnectingMQTT,
  Running,
  Error
};

struct AppState {
  State state;
  unsigned long lastTick;
};
```

State transition functions:
```cpp
AppState handleWiFi(AppState s) {
  if (WiFi.status() == WL_CONNECTED) {
    return { State::ConnectingMQTT, millis() };
  }
  return s;
}

AppState handleMQTT(AppState s) {
  if (mqttClient.connected()) {
    return { State::Running, millis() };
  }
  return s;
}

AppState runMainLogic(AppState s) {
  int value = analogRead(A0);
  mqttClient.publish("sensor", String(value).c_str());
  return s;
}
```

Dispatcher
```cpp
AppState dispatch(AppState s) {
  switch (s.state) {
    case State::ConnectingWiFi: return handleWiFi(s);
    case State::ConnectingMQTT: return handleMQTT(s);
    case State::Running:        return runMainLogic(s);
    default: return s;
  }
}
```

Main loop
```cpp
AppState appState = { State::ConnectingWiFi, millis() }; // Define starting point

void loop() {
  appState = dispatch(appState); // Start state machine
  mqttClient.loop();
  delay(500);
}
```

You now have a pure functional state machine—easy to reason about, test, and extend.

## 11. Monads (Maybe, Either)

See advanced section for details.

## 12. IO Monad / Effect System

See advanced section for details.
