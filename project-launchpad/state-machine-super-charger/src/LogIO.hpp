#pragma once

#include <Arduino.h>
#include <functional>

#include <FuncyControllerCPP.hpp> // Include the main library header
using namespace funcy_controller_cpp; // Use the library's namespace to avoid qualifying every type


IO<void> logIO(const String& msg);
