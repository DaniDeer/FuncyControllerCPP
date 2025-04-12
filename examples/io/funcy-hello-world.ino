#include <Arduino.h> // Requires Arduino framework context
#include <vector> // Required for std::vector

// My functional programming lib
#include <FuncyControllerCPP.hpp> // Include the main library header
using namespace funcy_controller_cpp; // Use the library's namespace to avoid qualifying every type

// Wrapper for Serial.println()
IO<void> logIO(String msg) {
//IO<void> logIO(const char* msg) { // See BUG down below.
  return IO<void>([=]() {
    Serial.println(msg);
  });
}

IO<void> printHello() {
  
  String msg1 = "Hello from ESP8266!";
  String msg2 = "My name is Daniel.";
  String msg3 = "How are you?";

  // BUG
  // You have to write unit() upfront when chaining
  // IO<void>, otherwise the first message will not
  // be written.
  // Alternatively use IO<void> logIO(const char* msg).
  IO<void> greetings = unit()
    .flatMap([msg1]() {  // This will not work and leads
      //return logIO(msg1); // to empty string in console
      return logIO("Hello from ESP8266!"); // but this works with unit() upfront
    })
    .flatMap([](String msg2 = "My name is Daniel.") {
      return logIO(msg2);
    })
    .flatMap([msg3]() {   // But this works
      return logIO(msg3);
    });
  
  greetings.run();

  msg1 = "Lets continue with fun facts!";
  msg2 = "Ahh got one!";

  // This runs perfeclty fine.
  logIO(msg1)
    .flatMap([](String msg2 = "Hmm lets see...") {
      return logIO(msg2);
    })
    .flatMap([=]() {
      return logIO(msg2);
    })
    .run();

  // SOLUTION for the bug?
  // Implement in the IO<void> template a default return unit()?
  return unit(); // return IO<void> but not greetings!

}

IO<void> printMyFavValue() {
  int value = 8;

  logIO(String(value) + " is my favourite number!").run();

  return unit();
}

IO<void> printGoodbye() {
  logIO("Nice to meet you.")
    .flatMap([]() {
      return logIO("See you again.");
    })
    .flatMap([]() {
      return logIO("Bye!");
    })
    .run();

  return unit();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:

  logIO("Hello World!").run();

  printHello().run();
  printMyFavValue().run();
  printGoodbye().run();

  delay(1000);
}
