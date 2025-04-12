#include <Arduino.h> // Requires Arduino framework context
// My functional programming lib
#include <FuncyControllerCPP.hpp> // Include the main library header
using namespace funcy_controller_cpp; // Use the library's namespace to avoid qualifying every type

// Declaration of global states/data first!

// =========== State Machine ===========

// State enum
enum class State {
  Init,
  Running,
  Step1,
  Error
};

// Function table
struct AppState {
  State state;
  unsigned long lastTick;
  String stateInfo;
};

// Wrapper for Serial.println()
IO<void> logIO(String msg) {
  return IO<void>([=]() {
    Serial.println(msg);
  });
}

// THEN definition of functions!

// Simulate IO<> which could have an side effect
IO<int> simpleIO() {
  int randVal = random(0, 2);  // 0 or 1
  return (randVal == 0) ? pure(42) : pure(-1);
}

// =========== State Definitions ===========

IO<AppState> init(AppState s) {

  // You can also define simpleIO in a function and execute:
  IO<int> simpleIO = IO<int>([]() {
    int randVal = random(0, 2);  // 0 or 1
    return (randVal == 0) ? 42 : -1;
  });

  int value = simpleIO.run();

  if (value > 0) {
    return pure( 
      AppState { 
        State::Running, 
        millis(), 
        "Initalization finished. Starting main." 
      }
    );
  }

  return pure(
    AppState { 
      State::Init, 
      millis(),
      "Error in init step. Start init again."
    }
  );

}

IO<AppState> mainProg(AppState s) {

  int value = simpleIO().run();

  if (value > 0) {
    return pure( 
      AppState { 
        State::Step1, 
        millis(), 
        "Main executed, start next step." 
      }
    );
  }

  return pure(
    AppState { 
      State::Error, 
      millis(),
      "Error in main step. Go into error handling."
    }
  );

}

IO<AppState> nextStep(AppState s) {
  
  int value = simpleIO().run();

  if (value > 0) {
    return pure(
      AppState { 
        State::Running, 
        millis(), 
        "Step 1 executed, start next main." 
      }
    );
  }

  s.lastTick = millis();
  s.stateInfo = "Error in step 1. Retry step 1.";

  return pure(s);

}

IO<AppState> error(AppState s) {
  
  int value = simpleIO().run();

  if (value > 0) {
    return pure(
      AppState { 
        State::Running, 
        millis(), 
        "Error handling successful. Starting main again." 
      }
    );
  }

  s.lastTick = millis();
  s.stateInfo = "Error handling failed. Starting error handling again.";

  return pure(s);

}

// =========== State Machine Dispatcher ===========

IO<AppState> dispatch(AppState s) {

  switch (s.state) {
    case State::Init: return init(s);
    case State::Running: return mainProg(s);
    case State::Step1: return nextStep(s);
    case State::Error: return error(s);
    default: return pure(s);
  }

}

// Define appState and init state to start with.
AppState appState = { State::Init, millis() };

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:

  appState = dispatch(appState).run(); 
  // gets back a IO<AppState> from dispatch() and runns the received effect with .run()
  logIO(appState.stateInfo).run(); // State Machine debugging, print state information

  delay(1000);

}

// WORK IN PROGRESS.....
// Goal: Fully functional state machine with Either monad to handle errors and state transitions
// Future: Async monad to handle async operations in the state machine.