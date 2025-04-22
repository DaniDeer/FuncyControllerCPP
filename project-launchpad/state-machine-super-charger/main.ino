// =========== Standard Libraries ===========
#include <Arduino.h> // Requires Arduino framework context

// =========== The FuncyControllerCPP Library ===========
// My functional programming lib
#include <FuncyControllerCPP.hpp> // Include the main library header
using namespace funcy_controller_cpp; // Use the library's namespace to avoid qualifying every type

// =========== Include /src files ===========
#include "WifiConnection.hpp"
#include "LogIO.hpp"

// =========== Global Definitions ===========

// Wifi Connection Config
const char* WIFI_SSID = "xxxx";
const char* WIFI_PASS = "xxxx";

WifiConnectionState WIFI_STATE; 
// Global state for Wifi connection
// I don't know if this is a good idea, or if there's a better way
// to store the state of the Wifi connection.

// =========== State Machine ===========

// State enum
enum class State {
  Init,
  Running,
  Error
};

// Function table
struct AppState {
  State state;
  unsigned long lastTick;
  String stateInfo;
};

// =========== State Definitions ===========

IO<AppState> init(AppState s) {

  String infoMsg = "Init.ConnectToWifi: Connecting to WiFi...";  
  Either<WifiConnectionState, WifiConnectionState> wifiState = logIO(infoMsg)
    .flatMap([]() {
      return connectToWifi(WIFI_SSID, WIFI_PASS);  // returns IO<Either<WifiConnectionState, WifiConnectionState>>
    })
    .run(); // IO<Either<...>> into Either<...> -> run effect (connect to WiFi)

  return wifiState.match(
    [=](WifiConnectionState wifiConnectionState) { // [=]: capture s for retrying connect to WiFi
      WIFI_STATE = wifiConnectionState; // Update the global WiFi state
      logIO("Init.ConnectToWifi: " + wifiConnectionState.statusMsg + " Retry connecting to Wifi...").run();
      return pure(s);  // Return the original state if there's an error and retry
    },
    [](WifiConnectionState wifiConnectionState) {
      WIFI_STATE = wifiConnectionState; // Update the global WiFi state
      logIO("Init.ConnectToWifi: " + wifiConnectionState.statusMsg).run();
      return pure( AppState{ State::Running, millis(), "Start main." } );  // Return the new state
    }
  );
}

IO<AppState> mainProg(AppState s) {

  return pure(
    AppState { 
      State::Error, 
      millis(),
      "Error in main step. Go into error handling."
    }
  );

}

IO<AppState> error(AppState s) {

  return pure(
      AppState { 
        State::Running, 
        millis(),
        "Error handling successful. Starting main again." 
      }
  );

}

// =========== State Machine Dispatcher ===========

IO<AppState> dispatch(AppState s) {

  switch (s.state) {
    case State::Init: return init(s);
    case State::Running: return mainProg(s);
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

  appState = dispatch(appState).run(); // State Machine Start
  logIO(appState.stateInfo).run(); // State Machine debugging

  delay(1000);

}