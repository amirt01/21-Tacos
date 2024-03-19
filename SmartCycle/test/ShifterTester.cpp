//
// Created by amirt on 3/10/2024.
//

#include <Arduino.h>

#include <Shifter.hpp>

#include "Button.hpp"

Button<13> up_shift_button{};
Button<12> down_shift_button{};

Shifter shifter{};

int encoder_value = 50;

void setup() {
  Serial.begin(115200);
}

void loop() {
  shifter.set_encoder_value(encoder_value);
  shifter.update();
  encoder_value += ceil(0.1 * shifter.get_motor_signal()) - random(0, 4) + random(0, 4);
  Serial.printf("current gear: %i\tencoder_value: %i\t", shifter.current_gear(), encoder_value);

  up_shift_button.update();
  down_shift_button.update();
  if (up_shift_button) {
    shifter.shift_up();
  } else if (down_shift_button) {
    shifter.shift_down();
  }
  Serial.print("\n");
  delay(100);
}
