//
// Created by amirt on 3/14/2024.
//

#include "Arduino.h"

#include "SmartCycleServer.hpp"

SmartCycleServer server;

void setup() {
  Serial.begin(115200);
  Serial.println();

  server.setup();
}

void loop() {
  server.set("speed", random(0, 45));
  server.set("cadence", random(0, 120));
  server.set("gear", random(1, 6));

  server.update();
}
