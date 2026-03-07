// include/CloudManager.h

#include <Arduino.h>
#pragma once

class CloudManager {
public:
  static void init();
  static bool sendEvent(const String &jsonPayload);
  static bool sendHeartbeat();
};