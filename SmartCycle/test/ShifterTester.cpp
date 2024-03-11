//
// Created by amirt on 3/10/2024.
//

#include <Arduino.h>

#include <Shifter.hpp>

Shifter shifter{};

uint16_t encoder_value = 50;

uint16_t shift_time = 5e3;

void setup() {
  Serial.begin(115200);
}

void loop() {
  shifter.set_encoder_value(encoder_value);
  shifter.update();
  encoder_value += ceil(0.1 * shifter.get_motor_signal()) - random(0, 4) + random(0, 4);
  Serial.print(shifter.current_gear());
  Serial.print('\t');
  Serial.println(encoder_value);
  if (millis() > shift_time) {
    Serial.print(shifter.shift_up() ? "true" : "false");
    shift_time += 5e3;
  }
  delay(100);
}
