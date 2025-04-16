# Async pattern

The Async pattern is used for handling asynchronous computations in a functional way. It allows you to represent computations that are delayed or computed in parallel.

## Use Cases

If you're dealing with async operations, such as network requests, file I/O, or background tasks, the Async pattern can help you compose these operations functionally.

## Class template ideation

### Async template

You can use the Async monad to compose asynchronous operations while keeping them pure and easy to reason about.

```cpp
template <typename T>
class Async {
public:
    using Func = std::function<std::future<T>()>;

    Async(Func func) : runFunc(func) {}

    std::future<T> run() const {
        return runFunc();
    }

    Async<T> map(std::function<T(T)> f) const {
        return Async<T>([=]() {
            return std::async([=]() {
                return f(runFunc().get());
            });
        });
    }

private:
    Func runFunc;
};
```

#### Example

```cpp
int main() {
    // Async example
    Async<int> async([]() {
        return std::async([]() {
            return 42;
        });
    });

    auto futureResult = async.run();
    int result = futureResult.get();

    // Test: Should print 42
    std::cout << "Async Test Result: " << result << std::endl;
    return 0;
}
```

#### Multi-Threading in Context of ESP8266 controllers

The Async pattern above uses `std::future` and `std::async`, which are part of the C++ Standard Library (specifically C++11 and beyond). Unfortunately, the ESP8266 and its toolchain do not fully support these features of the Standard Library, primarily because:

- **No Full C++ Standard Library Support**: The ESP8266 environment doesn't have full support for std::future, std::async, or other advanced multi-threading and concurrency features of C++11/14/17.

- **No Threading Support**: The ESP8266 runs on a single-core processor and doesn't support real threads like a general desktop environment would. It has cooperative multitasking using the `yield()` function, but there isn't a real thread pool or concurrency as you'd find in more capable systems.

>Workaround: One approach is to simulate async behavior using `millis()` or the `yield()` function, which allows you to periodically check if an asynchronous task has completed, without blocking the main loop

The Async pattern in the ESP8266 world allows you to handle non-blocking tasks (like network requests or delays) without freezing the microcontroller’s main loop. Using `Promises` and `Callbacks` allows for clean and efficient asynchronous flow without blocking the system.

Check the following ideas:

### Implementing Async Pattern with Promises and Future

- A Promise is a built-in abstraction in many programming languages (like modern C++ with `std::future`), which represents a value that will be available in the future (or an error that might happen).
- Promises allow you to attach `.then()` handlers to specify what happens when the asynchronous operation resolves (either with a result or an error). Promises are usually designed to work with multi-threaded environments or with an event loop, and they handle callbacks internally.
- Promises represent the eventual result of an asynchronous operation, while Async in your example simulates async behavior using a single-threaded, non-blocking approach.


```cpp
template <typename T>
class Promise {
public:
  using Callback = std::function<void(T)>;

  Promise() : isResolved(false) {}

  void resolve(T value) {
    if (!isResolved) {
      result = value;
      isResolved = true;
      if (callback) {
        callback(value);
      }
    }
  }

  void then(Callback callbackFn) {
    callback = callbackFn;
    if (isResolved) {
      callback(result);
    }
  }

private:
  T result;
  bool isResolved;
  Callback callback;
};
```

Promise Class:
- A simple class that stores the result of an asynchronous operation.
- It has a `resolve()` method to set the result and trigger the callback.
- The `then()` method allows you to attach a callback that will be called once the operation completes.

#### Example of Promise

```cpp
// Example Async function
Promise<int> asyncDelay(int value, unsigned long delayMs) {
  Promise<int> promise;

  // Simulate async behavior (non-blocking)
  unsigned long startTime = millis();

  // In the loop, check if enough time has passed for the delay
  if (millis() - startTime > delayMs) {
    promise.resolve(value);  // Resolve the promise when the time is up
  }

  return promise;
}

void setup() {
  Serial.begin(115200);

  // Example usage of asyncDelay
  auto promise = asyncDelay(42, 1000);  // Delay for 1000ms

  // Define the callback for when the promise is resolved
  promise.then([](int result) {
    Serial.print("Async result: ");
    Serial.println(result);
  });
}

void loop() {
  // Keep the loop running, promise resolution happens asynchronously
}
```

Async Function:
- `asyncDelay` simulates a non-blocking task that resolves after a delay (this could be a network request, sensor reading, etc.).
- It uses `millis()` to simulate an asynchronous delay (in a real-world scenario, this could be a non-blocking network call, etc.).

Usage:
- In the `setup()`, the asyncDelay function is called with a `Promise<int>`.
- The `then()` method is used to define the callback that handles the result once the promise is resolved (after 1000ms).

### Async with Callback-Based System (Non-blocking)

```cpp
class AsyncTask {
public:
  using Callback = std::function<void(int)>;

  AsyncTask() : taskInProgress(false) {}

  void startTask(Callback callback) {
    taskInProgress = true;
    // Simulate an async operation (e.g., network request or delay)
    unsigned long startTime = millis();
    while (millis() - startTime < 1000) { // Simulating a non-blocking wait (1000 ms)
      yield(); // Yield to keep the ESP8266 responsive
    }
    taskInProgress = false;

    // After task completes, call the callback with the result
    callback(42);  // Simulated result (42)
  }

private:
  bool taskInProgress;
};

```

#### Example of Callback

```cpp
void setup() {
  Serial.begin(115200);
  
  AsyncTask asyncTask;
  asyncTask.startTask([](int result) {
    Serial.print("Async task completed with result: ");
    Serial.println(result);
  });
}

void loop() {
  // Keep the loop running, the task will complete asynchronously
}
```

- `yield()`: Used in the loop to prevent blocking and keep the ESP8266 responsive.`
- Callback: Once the async task completes (simulated with millis()), the callback is called with the result.

#### Async HTTP Request (with Callback)

```cpp
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

class AsyncHTTP {
public:
  using Callback = std::function<void(String)>;

  void getAsync(const String& url, Callback callback) {
    WiFiClient client;
    HTTPClient http;
    
    http.begin(client, url);
    int httpCode = http.GET();

    String response;
    if (httpCode > 0) {
      response = http.getString();
    } else {
      response = "Error on HTTP request";
    }

    http.end();

    // Call the callback with the response
    callback(response);
  }
};

void setup() {
  Serial.begin(115200);
  WiFi.begin("SSID", "PASSWORD");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  AsyncHTTP asyncHttp;
  asyncHttp.getAsync("http://example.com", [](String response) {
    Serial.println("Async HTTP response: ");
    Serial.println(response);
  });
}

void loop() {
  // Keep the loop running while the async task completes
}
```

- `AsyncHTTP` class: Uses HTTPClient to perform an HTTP GET request asynchronously and returns the result using a callback function.
- `getAsync method`: Initiates the HTTP request and resolves the response through the callback.


## Other Ideas for Async:

- Integrate with `Ticker` or hardware timers for true async.
- Extend with cancellation. -> missing in ideas
- Create `Async<Either<T, E>>` for `TaskEither` behavior.
- Add support for error handling via `AsyncResult<T, E>`.
- Combine <IO> and TaskEither for blocking and non-blocking operations. -> missing in ideas

### Ticker

`Ticker` allows you to schedule a collback to run after a delay or at regular intervals without blocking the main loop.

```cpp	
#include <Ticker.h>
```

Async<T> with Ticker (based on template below):
```cpp
template<typename T>
Async<T> fromValueDelayed(T value, unsigned long ms) {
  return Async<T>([=](std::function<void(T)> cb) {
    Ticker* t = new Ticker();
    t->once_ms(ms, [=]() {
      cb(value);
      delete t;
    });
  });
}
```

Full Async<T> monad recap:
```cpp
template<typename T>
class Async {
public:
  using Callback = std::function<void(T)>;
  using Effect = std::function<void(Callback)>;

  explicit Async(Effect effect) : effect(effect) {}

  void run(Callback cb) const {
    effect(cb);
  }

  template<typename F, typename U = decltype(std::declval<F>()(std::declval<T>()))>
  Async<U> map(F f) const {
    return Async<U>([=](std::function<void(U)> cb) {
      this->run([=](T val) {
        cb(f(val));
      });
    });
  }

  template<typename F, typename AsyncU = decltype(std::declval<F>()(std::declval<T>()))>
  Async<typename AsyncU::value_type> flatMap(F f) const {
    return Async<typename AsyncU::value_type>([=](std::function<void(typename AsyncU::value_type)> cb) {
      this->run([=](T val) {
        f(val).run(cb);
      });
    });
  }

  using value_type = T;

private:
  Effect effect;
};
```

Usage:
```cpp
void setup() {
  Serial.begin(115200);

  auto asyncTask = delayAsync(1000)
    .flatMap([]() {
      return fromValueDelayed<String>("Hello after delay!", 500);
    });

  asyncTask.run([](String message) {
    Serial.println(message);
  });
}

void loop() {
  // Main loop remains responsive
}
```

### TaskEither<T, E> - composition of Async and Either

`TaskEither<T, E>` represesnts a computation that:
- Runs asynchronously
- Might fail with an error of type `E`
- Returns a value of type `T` on success
It is essentially `Async<Either<T, E>>`

#### Type Alias
For readability
```cpp
template<typename T, typename E>
using TaskEither = Async<Either<T, E>>;
```

#### Helper rightTask and leftTask
Lift a pure value or error into TaskEither.

```cpp
template<typename T, typename E>
TaskEither<T, E> rightTask(T value) {
  return TaskEither<T, E>([=](std::function<void(Either<T, E>)> cb) {
    cb(Either<T, E>::Right(value));
  });
}

template<typename T, typename E>
TaskEither<T, E> leftTask(E error) {
  return TaskEither<T, E>([=](std::function<void(Either<T, E>)> cb) {
    cb(Either<T, E>::Left(error));
  });
}
```

#### map and mapLeft for TaskEither
```cpp
template<typename T, typename E, typename F>
TaskEither<decltype(std::declval<F>()(std::declval<T>())), E>
mapTask(TaskEither<T, E> task, F f) {
  return task.map([=](Either<T, E> e) {
    return e.map(f);
  });
}

template<typename T, typename E, typename F>
TaskEither<T, decltype(std::declval<F>()(std::declval<E>()))>
mapLeftTask(TaskEither<T, E> task, F f) {
  return task.map([=](Either<T, E> e) {
    return e.mapLeft(f);
  });
}
```

#### flatMap for TaskEither
If Right -> run next async task
If Left ->  short circuit and return the error
```cpp
template<typename T, typename E, typename F, typename U = typename decltype(std::declval<F>()(std::declval<T>()))::value_type::value_type>
TaskEither<U, E> flatMapTask(TaskEither<T, E> task, F f) {
  return TaskEither<U, E>([=](std::function<void(Either<U, E>)> cb) {
    task.run([=](Either<T, E> result) {
      if (result.isRight()) {
        f(result.unwrapRight()).run(cb);
      } else {
        cb(Either<U, E>::Left(result.unwrapLeft()));
      }
    });
  });
}
```

#### Example of TaskEither

```cpp
TaskEither<String, String> delayedCheck(bool success) {
  return TaskEither<String, String>([=](std::function<void(Either<String, String>)> cb) {
    Ticker* t = new Ticker();
    t->once_ms(1000, [=]() {
      if (success) {
        cb(Either<String, String>::Right("All good!"));
      } else {
        cb(Either<String, String>::Left("Oops, something went wrong"));
      }
      delete t;
    });
  });
}

void setup() {
  Serial.begin(115200);

  delayedCheck(true)
    .map([](Either<String, String> result) {
      return result.map([](String msg) {
        return msg + " (logged)";
      });
    })
    .run([](Either<String, String> result) {
      result.match(
        [](String err) { Serial.println("Error: " + err); },
        [](String val) { Serial.println("Success: " + val); }
      );
    });
}

void loop() {
  // remains responsive
}
```

#### Conclusion TaskEither Class Template and Helper

```cpp
template<typename T, typename E>
class TaskEither {
public:
  using value_type = T;
  using error_type = E;
  using either_type = Either<T, E>;
  using Callback = std::function<void(either_type)>;

  TaskEither(std::function<void(Callback)> thunk)
    : runAsync(thunk) {}

  // Run the async task
  void run(Callback cb) const {
    runAsync(cb);
  }

  // map: transform Right
  template<typename F>
  auto map(F f) const -> TaskEither<decltype(f(std::declval<T>())), E> {
    using U = decltype(f(std::declval<T>()));
    return TaskEither<U, E>([=](std::function<void(Either<U, E>)> cb) {
      run([=](either_type result) {
        cb(result.map(f));
      });
    });
  }

  // mapLeft: transform Left
  template<typename F>
  auto mapLeft(F f) const -> TaskEither<T, decltype(f(std::declval<E>()))> {
    using E2 = decltype(f(std::declval<E>()));
    return TaskEither<T, E2>([=](std::function<void(Either<T, E2>)> cb) {
      run([=](either_type result) {
        cb(result.mapLeft(f));
      });
    });
  }

  // flatMap: chain TaskEithers
  template<typename F, typename U = typename decltype(std::declval<F>()(std::declval<T>()))::value_type>
  auto flatMap(F f) const -> TaskEither<typename U::value_type, E> {
    using UVal = typename U::value_type;
    return TaskEither<UVal, E>([=](std::function<void(Either<UVal, E>)> cb) {
      run([=](either_type result) {
        if (result.isRight()) {
          f(result.unwrapRight()).run(cb);
        } else {
          cb(Either<UVal, E>::Left(result.unwrapLeft()));
        }
      });
    });
  }

  String toString() const {
    return "TaskEither<T, E>";
  }

private:
  std::function<void(Callback)> runAsync;
};

// Helper functions to create TaskEither instances
template<typename T, typename E>
TaskEither<T, E> rightTask(T value) {
  return TaskEither<T, E>([=](std::function<void(Either<T, E>)> cb) {
    cb(Either<T, E>::Right(value));
  });
}

template<typename T, typename E>
TaskEither<T, E> leftTask(E error) {
  return TaskEither<T, E>([=](std::function<void(Either<T, E>)> cb) {
    cb(Either<T, E>::Left(error));
  });
}
```

Example use with Ticker:

```cpp
TaskEither<String, String> fetchSensorData() {
  return TaskEither<String, String>([](std::function<void(Either<String, String>)> cb) {
    Ticker* t = new Ticker();
    t->once_ms(500, [=]() {
      // simulate success
      cb(Either<String, String>::Right("Temperature: 23.4°C"));
      delete t;
    });
  });
}

void setup() {
  Serial.begin(115200);

  fetchSensorData()
    .map([](String val) {
      return val + " (checked)";
    })
    .flatMap([](String checkedVal) {
      return rightTask<String, String>("Final: " + checkedVal);
    })
    .run([](Either<String, String> result) {
      result.match(
        [](String err) { Serial.println("Failed: " + err); },
        [](String ok)  { Serial.println("Success: " + ok); }
      );
    });
}

void loop() {}
  // Keep the loop running
```

### IO<T> and TaskEither<T, E>
- bridge blocking and non-blocking