//
// Created by amirt on 3/13/2024.
//

#include <arduino.h>

#include "ButtonHandler.hpp"

static constexpr uint8_t BUTTON_PIN{13};
Button<BUTTON_PIN> bh{};

bool test_flag;

void setup() {
  Serial.begin(115200);
}

void loop() {
  bh.loop();

  Serial.printf("%lu\tbutton state: %s\n", millis(), bh.to_str().data());
}
