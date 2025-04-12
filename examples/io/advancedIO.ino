#include <Arduino.h> // Requires Arduino framework context
#include <vector> // Required for std::vector

// My functional programming lib
#include <FuncyControllerCPP.hpp> // Include the main library header
using namespace funcy_controller_cpp; // Use the library's namespace to avoid qualifying every type

// pure() - Lift a value into IO<T>
// testPure(); // expected output: pure() -> IO<int> result: 42
void testPure() {
  IO<int> ioVal = pure(42);
  int result = ioVal.run();
  Serial.println("pure() -> IO<int> result: " + String(result));
}

// liftIO() - Turn an IO<T> into an IO<Either<T, E>> (assumes success)
// testLiftIO(); // expected output: liftIO() success: 123
void testLiftIO() {
  IO<int> simpleIO = IO<int>([]() {
    return 123;
  });

  IO<Either<int, String>> lifted = liftIO<int, String>(simpleIO);
  Either<int, String> result = lifted.run();

  if (result.isRight()) {   // Success is output Right
    Serial.println("liftIO() success: " + String(result.unwrapRight()));
  }

}

// liftIO() - Turn an IO<T> into an IO<Either<T, E>> (assumes success)
// testLiftIO(); // expected output: liftIO() success: 123
void testLiftIOwithIOvoid() {
  IO<int> simpleIO = IO<int>([]() {
    return 123;
  });

  IO<void> logIO = IO<void>([]() {
    Serial.println("Info Message from testLiftIOwithIOvoid");
  });

  // Chain IO<T>, with IO<void>
  // ATTENTION: Sequence order matters here!
  // last return IO<int> simpleIO goes into IO<int> toBeLifted
  IO<int> toBeLifted = logIO
    .flatMap([=]() {
      return simpleIO;
    });

  IO<Either<int, String>> lifted = liftIO<int, String>(toBeLifted);
  Either<int, String> result = lifted.run();

  if (result.isRight()) {   // Success is output Right
    Serial.println("testLiftIOwithIOvoid() success: " + String(result.unwrapRight()));
  }

}

void testSequencingOperators() {
  Serial.println("\n--- Testing Sequencing Operators ---");

  IO<int> simpleIO = IO<int>([]() { Serial.println("Running simpleIO"); return 123; });
  IO<void> logIO = IO<void>([]() { Serial.println("Running logIO"); });
  IO<String> stringIO = IO<String>([](){ Serial.println("Running stringIO"); return String("done"); });

  // Case 1: IO<T> -> IO<void> -> IO<T> (Keep T) - Use thenKeep
  Serial.println("\nTesting simpleIO.thenKeep(logIO):");
  IO<int> result1 = simpleIO.thenKeep(logIO);
  Serial.print("Result 1 value: "); Serial.println(result1.run()); // Runs simpleIO, then logIO, returns 123

  // Case 2: IO<void> -> IO<T> -> IO<T> - Use then
  Serial.println("\nTesting logIO.then(simpleIO):");
  IO<int> result2 = logIO.then(simpleIO);
  Serial.print("Result 2 value: "); Serial.println(result2.run()); // Runs logIO, then simpleIO, returns 123
  // Note: This is equivalent to your working flatMap example: logIO.flatMap([=](){ return simpleIO; })

  // Case 3: IO<T> -> IO<U> -> IO<U> (Discard T) - Use then
  Serial.println("\nTesting simpleIO.then(stringIO):");
  IO<String> result3 = simpleIO.then(stringIO);
  Serial.print("Result 3 value: "); Serial.println(result3.run()); // Runs simpleIO, then stringIO, returns "done"

   // Case 4: IO<void> -> IO<void> -> IO<void> - Use then
  Serial.println("\nTesting logIO.then(logIO):");
  IO<void> result4 = logIO.then(logIO);
  result4.run(); // Runs logIO, then logIO again

  Serial.println("--- Sequencing Tests Done ---");
}

// Like liftIO() - Turn an IO<T> into an IO<Either<T, E>> (assumes success)
// But with thenKeep() the sequence does not matter!
void testLiftIOwithIOvoidSequencing() {
  IO<int> simpleIO = IO<int>([]() {
    return 123;
  });

  IO<void> logIO = IO<void>([]() {
    Serial.println("Info Message from testLiftIOwithIOvoid");
  });

  // Chain IO<T>, with IO<void>
  // Its much easier with this new thenKeep() function!
  IO<int> toBeLifted = simpleIO.thenKeep(logIO);

  IO<Either<int, String>> lifted = liftIO<int, String>(toBeLifted);
  Either<int, String> result = lifted.run();

  if (result.isRight()) {   // Success is output Right
    Serial.println("testLiftIOwithIOvoid() success: " + String(result.unwrapRight()));
  }

}

// liftIOLeft() - Turn an IO<T> into an IO<Either<T, E>> (assumes error)
// testLiftIOLeft(); // expected output: liftIOLeft() error: 123
void testLiftIOLeft() {
  IO<int> simpleIO = IO<int>([]() {
    return 123;
  });

  IO<Either<String, int>> lifted = liftIOLeft<String, int>(simpleIO);
  Either<String, int> result = lifted.run();

  if (result.isLeft()) {
    Serial.println("liftIOLeft() error: " + String(result.unwrapLeft()));
  }

}

// unit() - IO<void> that does nothing, but is chainable
// testUnit(); // expected output: unit() then -> Hello from next effect
void testUnit() {
  IO<void> doNothing = unit();

  doNothing
    .flatMap([]() {
      return IO<void>([]() {
        Serial.println("unit() then -> Hello from next effect");
      });
    })
    .run();
}

// Error Simulation for liftIO()
// expected output: liftIO() error: 42; liftIOLeft() error: -1
void testLiftIOandLiftIOLeft() {

  // Simulate IO<> which could have an side effect
  IO<int> simpleIO = IO<int>([]() {
    int randVal = random(0, 2);  // 0 or 1
    return (randVal == 0) ? 42 : -1;
  });

  IO<Either<int, int>> lifted = IO<Either<int, int>>([]() { return Either<int, int>::Right(0); });

  int randomResult = simpleIO.run();

  if (randomResult > 0) {
    lifted = liftIO<int, int>(pure(randomResult));
  } else {
    lifted = liftIOLeft<int, int>(pure(randomResult));
  }

  Either<int, int> result = lifted.run();

  if (result.isRight()) {   // Success is output Right
    Serial.println("liftIO() success: " + String(result.unwrapRight()));
  } else {  // result.isLeft() is true
    Serial.println("liftIOLeft() error: " + String(result.unwrapLeft()));
  }

}

void testLiftIOandLiftIOLeftButUseMap() {
  IO<int> simpleIO = IO<int>([]() {
    int randVal = random(0, 2);  // 0 or 1
    return (randVal == 0) ? 42 : -1;
  });

  // Chain everything without calling .run() prematurely
  IO<Either<int, int>> resultIO = simpleIO.map([](int val) {
    return (val > 0)
      ? Either<int, int>::Right(val)
      : Either<int, int>::Left(val);
  });

  // Run to get the actual Either
  // 🎯 Remember: IO<Either<T, E>> is not Either<T, E> — call .run() before using match().
  Either<int, int> result = resultIO.run();
  // OR alternatively run Either<int, int>> resultIO = like follows:
  /*
  Either<int, int>> result = simpleIO.map([](int val) {
    return (val > 0)
      ? Either<int, int>::Right(val)
      : Either<int, int>::Left(val);
  })
  .run();
  */
  
  // Now run and match at the end
  result.match(
    [](int err) {
      Serial.println("❌ Error: " + String(err));
    },
    [](int ok) {
      Serial.println("✅ Success: " + String(ok));
    }
  );

}

// liftIOtoEither(): reduces code for same functionality as in 
// testLiftIOandLiftIOLeft() and testLiftIOandLiftIOLeftButUseMap()
// Wraps T in Either, applying error-checking logic
void testLiftIOtoEither() {
  IO<int> simpleIO = IO<int>([]() { return random(0, 2) ? 42 : -1; });

  IO<Either<int, int>> lifted = liftIOtoEither<int, int>(
    simpleIO,
    [](int val) { return val; },         // identity as error
    [](int val) { return val <= 0; }     // error condition
  );

  Either<int, int> result = lifted.run();
  result.match(
    [](int err) { Serial.println("Error: " + String(err)); },
    [](int ok)  { Serial.println("Success: " + String(ok)); }
  );

}

// flatMapIOEither(): 
// expected output: Sensor Value: 42; randVal == 0 -> nothing printed
// It onlly executes the chained function (logValue) if the Either is a Right
void testFlatMapIOEither() {
  IO<Either<int, String>> simpleIO = IO<Either<int, String>>([]() { 
    int randVal = random(0, 2) ? 42 : -1;
    if (randVal > 0) return Either<int, String>::Right(randVal);
    return Either<int, String>::Left("Sensor read failed");
  });

  // IO<Either<int, String>> even when using IO<void>
  auto logValue = [](int val) -> IO<Either<int, String>> {
    IO<void> log = IO<void>([=]() {
      Serial.println("Sensor Value: " + String(val));
    });
    return liftVoidIO<int, String>(log);  // lifts IO<void> to IO<Either<int, String>>
  };

  // Chaining using flatMapIOEither
  auto fullFlow = flatMapIOEither<int, String>(simpleIO, logValue);
  fullFlow.run(); // Run the effect

}

// mapIOEither() and mapLeftIOEither(): expected output:
// if randVal > 0
// transformedRight.fold: Right result: Transformed value: 84
// transformedLeft.fold: Right result: 42
// if randVal <= 0
// transformedRight.fold: Left result: Sensor read failed
// transformedLeft.fold: Left result: Error occurred: Sensor read failed
// Bug: The output is mixed up, but the implementation of mapIOEither and is
// mapLeftIOEither tested.
void testMapIOEitherAndMapLeftIOEither() {
  // Simulate IO computation that produces Either
  IO<Either<int, String>> simpleIO = IO<Either<int, String>>([]() { 
    int randVal = random(0, 2) ? 42 : -1;
    if (randVal > 0) return Either<int, String>::Right(randVal);
    return Either<int, String>::Left("Sensor read failed");
  });

  // Map for transforming Right value (ensure the output is String)
  auto transformRight = [](int val) -> String {
    return "Transformed value: " + String(val * 2);  // Double the value and return as String
  };

  // Map for transforming Left value and let the Right be unchanged(int)
  auto transformLeft = [](String error) -> String {
    return "Error occurred: " + error;
  };

  // Apply mapIOEither to transform the Right value (which switches from int to String)
  IO<Either<String, String>> transformedRight = mapIOEither(simpleIO, transformRight);

  // Apply mapLeftIOEither to transform the Left value (which do not change Right(int))
  IO<Either<int, String>> transformedLeft = mapLeftIOEither(simpleIO, transformLeft);

  // Fold the results to print
  transformedRight.run().fold(
    [](String error) { 
      Serial.println("Left result: " + error); 
    },
    [](String value) { 
      Serial.println("Right result: " + value); 
    }
  );

  transformedLeft.run().fold(
    [](String error) { 
      Serial.println("Left result: " + error); 
    },
    [](int value) { 
      Serial.println("Right result: " + String(value)); 
    }
  );

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:

  testPure();
  testLiftIO();
  testLiftIOwithIOvoid();
  testLiftIOwithIOvoidSequencing();
  testLiftIOLeft();
  testLiftIOandLiftIOLeft();
  testLiftIOandLiftIOLeftButUseMap();
  testLiftIOtoEither();
  testFlatMapIOEither();
  testMapIOEitherAndMapLeftIOEither();

  delay(1000);
}
