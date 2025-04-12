# FuncyControllerCPP

_Write embedded code in a func(y) way in CPP!_

**Version:** 0.1.0  
**Author:** Daniel Hirsch (<danirocks@hotmail.de>)

## Overview

FuncyControllerCPP is a header-only C++ library that brings functional programming patterns like `Maybe`, `Either`, `IO`, and `Async` to microcontrollers. It is designed for platforms like ESP8266, ESP32, and other Arduino-compatible devices, enabling developers to write cleaner, safer, and more expressive code - the func(y) way.

## Features

- **Maybe Monad**: Handle optional values without null checks or exceptions.
- **Either Monad**: Represent computations that can succeed or fail.
- **IO Monad**: Encapsulate side effects in a functional way.
- **Async Monad**: Manage asynchronous operations with ease.
- **Functional Helpers**: Utilities for composing and transforming monads.

## Use Cases

- Simplify error handling with `Either`.
- Safely handle optional values with `Maybe`.
- Encapsulate side effects like logging or hardware interactions using `IO`.
- Manage asynchronous tasks with `Async` (WIP, in the future).

## Installation

1. Clone or download the repository.
2. Copy the `src` folder into your Arduino `libraries` directory.
3. Include the library in your Arduino project:

```cpp
#include <FuncyControllerCPP.hpp> // Include the main library header
using namespace funcy_controller_cpp; // Use the library's namespace to avoid qualifying every type
```

## Examples

Basic Maybe usage

```cpp
#include <FuncyControllerCPP.hpp>
using namespace funcy_controller_cpp;

Maybe<int> findFirstEven(const std::vector<int>& numbers) {
    for (int num : numbers) {
        if (num % 2 == 0) {
            return Maybe<int>::Just(num);
        }
    }
    return Maybe<int>::Nothing();
}
```

IO Monad example

```cpp
#include <FuncyControllerCPP.hpp>
using namespace funcy_controller_cpp;

IO<void> logMessage(String msg) {
    return IO<void>([=]() {
        Serial.println(msg);
    });
}

void setup() {
    Serial.begin(115200);
    logMessage("Hello, world!").run();
}
```

A little more func(y) - chaining functions

```cpp
#include <FuncyControllerCPP.hpp>
using namespace funcy_controller_cpp;

IO<void> logIO(String msg) {
  return IO<void>([=]() {
    Serial.println(msg);
  });
}

IO<int> simpleIO() {
  int randVal = random(0, 2);  // 0 or 1
  return (randVal == 0) ? pure(42) : pure(-1);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:

  /*
  int result;
  result = simpleIO().run();
  logIO(String(result)).run();
  */
  // This can also be written as:

  simpleIO()
    .map([](int result) {
      return String(result);
    })
    .flatMap([](String result) {
      return logIO("Result: " + result);
    })
    .run();

  delay(1000);

}
```

For more examples, check the `examples` folder.

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
