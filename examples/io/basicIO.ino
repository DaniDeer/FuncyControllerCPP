#include <Arduino.h> // Requires Arduino framework context
// My functional programming lib
#include <FuncyControllerCPP.hpp> // Include the main library header
using namespace funcy_controller_cpp; // Use the library's namespace to avoid qualifying every type

// Wrapper for Serial.println()
IO<void> logIO(String msg) {
  return IO<void>([=]() {
    Serial.println(msg);
  });
}

/*
IO<int> simpleIO() {
  int randVal = random(0, 2);  // 0 or 1

  if (randVal == 0) {
    randVal = 42;
    return pure(randVal);
  }

  randVal = -1;
  return pure(randVal);

}
*/
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
