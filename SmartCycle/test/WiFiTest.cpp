//
// Created by amirt on 3/14/2024.
//

#include "Arduino.h"

#include "SmartCycleServer.hpp"

auto& server = SmartCycleServer::get_instance();

void setup() {
  Serial.begin(115200);
  Serial.println();

  server.get_msg_ref().speed = random(0, 45);
  server.get_msg_ref().cadence = random(0, 120);
  server.get_msg_ref().current_gear = random(1, 6);

  server.setup();
}

void loop() {
  server.loop();
}
