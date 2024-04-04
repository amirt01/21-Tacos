//
// Created by amirt on 3/10/2024.
//

#include <Arduino.h>
#include "FastLED.h"

#include "Shifter.hpp"

#include "ButtonHandler.hpp"

Button<13> up_shift_button{};
Button<14> down_shift_button{};

Shifter shifter{};

CRGB leds[6]{};

short encoder_value = 50;

void setup() {
  Serial.begin(115200);

  CFastLED::addLeds<WS2813, 12, GRB>(leds, Shifter::MAX_GEAR);
  FastLED.clear(true);
}

void loop() {
  shifter.set_encoder_value(encoder_value);
  shifter.loop();
  encoder_value += ceil(0.1 * shifter.get_motor_signal()) - random(0, 4) + random(0, 4);
  Serial.printf("current gear: %i\tencoder_value: %i\t", shifter.get_current_gear(), encoder_value);

  up_shift_button.loop();
  down_shift_button.loop();
  if (up_shift_button) {
    shifter.shift_up();
  } else if (down_shift_button) {
    shifter.shift_down();
  }
  Serial.print("\n");

  FastLED.clear();
  leds[shifter.get_target_gear() - 1] = CRGB::Blue;
  leds[shifter.get_current_gear() - 1] = CRGB::Green;
  FastLED.show();
  delay(100);
}
