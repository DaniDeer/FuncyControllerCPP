#pragma once

#include <ESP8266WiFi.h>
#include <FuncyControllerCPP.hpp> // Include the main library header
using namespace funcy_controller_cpp; // Use the library's namespace to avoid qualifying every type

enum class WifiStatus {
  WL_CONNECTED,       // 0
  WL_NO_SSID_AVAIL,   // 1
  WL_CONNECT_FAILED,  // 2
  WL_WRONG_PASSWORD,  // 3
  WL_DISCONNECTED,    // 4
  WL_UNKNOWN          // 5
};

struct WifiConnectionState {
  Maybe<String> ipAddr; // IP address if connected
  // Using the Maybe<T> type to represent an optional value
  WifiStatus wifiStatus;
  String statusMsg;

  // Default constructor for initializing the state
  // so that Either can use {} when constructing the state
  WifiConnectionState()
    : ipAddr(Maybe<String>::Nothing()), wifiStatus(WifiStatus::WL_UNKNOWN), statusMsg("") {}

  WifiConnectionState(Maybe<String> ip, WifiStatus status, const String& msg)
    : ipAddr(ip), wifiStatus(status), statusMsg(msg) {}
};

struct WifiConnectConfig {
  unsigned long timeoutMs;       // How long to wait before giving up
  unsigned long retryIntervalMs; // Delay between connection status checks

  WifiConnectConfig(unsigned long timeout = 15000, unsigned long interval = 500) // Default 15 seconds, 500ms interval
    : timeoutMs(timeout), retryIntervalMs(interval) {}
};

IO<Either<WifiConnectionState, WifiConnectionState>> connectToWifi(const String& ssid, const String& password, const WifiConnectConfig& config = WifiConnectConfig());
