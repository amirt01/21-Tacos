//
// Created by amirt on 3/13/2024.
//

#include <arduino.h>

#include "ButtonHandler.hpp"

static constexpr uint8_t BUTTON_PIN{13};

ButtonHandler bh{};

bool test_flag;

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  bh.update(millis(), digitalRead(BUTTON_PIN));

  Serial.printf("%lu\tbutton state: %s\n", micros(), bh.get_button_state_string().c_str());
}
