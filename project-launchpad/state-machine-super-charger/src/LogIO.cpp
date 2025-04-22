#include "LogIO.hpp"

// Wrapper for Serial.println()
IO<void> logIO(const String& msg) {
  return IO<void>([=]() {
    Serial.println(msg);
  });
}
