//
// Created by amirt on 3/13/2024.
//

#include <arduino.h>

#include "ButtonHandler.hpp"

static constexpr uint8_t BUTTON_PIN{13};
ButtonHandler<BUTTON_PIN> bh{};

bool test_flag;

void setup() {
  Serial.begin(115200);
}

void loop() {
  bh.update();

  Serial.printf("%lu\tbutton state: %s\n", millis(), bh.get_button_state_string().c_str());
}
