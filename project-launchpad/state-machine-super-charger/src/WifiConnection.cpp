#include "WifiConnection.hpp"

// Status mapping helper function 
// Converts wl_status_t from ESP8266WiFi to WifiStatus wrapper enum
WifiStatus toWifiStatus(wl_status_t status) {
  switch (status) {
    case WL_CONNECTED:      return WifiStatus::WL_CONNECTED;      // 0
    case WL_NO_SSID_AVAIL:  return WifiStatus::WL_NO_SSID_AVAIL;  // 1
    case WL_CONNECT_FAILED: return WifiStatus::WL_CONNECT_FAILED; // 2
    case WL_WRONG_PASSWORD: return WifiStatus::WL_WRONG_PASSWORD; // 3
    case WL_DISCONNECTED:   return WifiStatus::WL_DISCONNECTED;   // 4
    default:                return WifiStatus::WL_UNKNOWN;        // 5
  }
}

// Converts WifiStatus enum to a human-readable description
// for logging and debugging messages
String describe(WifiStatus status) {
  switch (status) {
    case WifiStatus::WL_CONNECTED:      return "Successfully connected to WiFi.";
    case WifiStatus::WL_NO_SSID_AVAIL:  return "SSID not available.";
    case WifiStatus::WL_CONNECT_FAILED: return "Connection failed.";
    case WifiStatus::WL_WRONG_PASSWORD: return "Wrong password.";
    case WifiStatus::WL_DISCONNECTED:   return "Disconnected from network.";
    case WifiStatus::WL_UNKNOWN:        return "Unknown WiFi status.";
    default:                            return "Unknown WiFi status.";
  }
}

// IO<Either<T, E>> wrapper for ESP8266 WiFi connection
IO<Either<WifiConnectionState, WifiConnectionState>> connectToWifi(
  const String& ssid, 
  const String& password,
  const WifiConnectConfig& config
) {
  return IO<Either<WifiConnectionState, WifiConnectionState>>([=]() {
    //WifiStatus = wifiStatus::WL_UNKNOWN;

    WiFi.begin(ssid.c_str(), password.c_str());
    unsigned long startAttemptTime = millis();
    //wifiStatus = WiFi.status(); call status after begin?
    // Wait for connection, with a timeout of 15 seconds
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < config.timeoutMs) {
      delay(config.retryIntervalMs);
    }

    wl_status_t rawStatus = WiFi.status();
    WifiStatus status = toWifiStatus(rawStatus);

    // Check if the connection was successful (Right side of Either)
    if (status == WifiStatus::WL_CONNECTED) {
      String ippAddr = WiFi.localIP().toString();
      return Either<WifiConnectionState, WifiConnectionState>::Right(
        WifiConnectionState{
          Maybe<String>::Just(ippAddr),
          status,
          describe(status) + " IP: " + ippAddr
        }
      );
    }

    // Handle the case where the connection attempt failed (left side of Either)
    return Either<WifiConnectionState, WifiConnectionState>::Left(
      WifiConnectionState{
        Maybe<String>::Nothing(),
        status,
        describe(status)
      }
    );

  });
}
