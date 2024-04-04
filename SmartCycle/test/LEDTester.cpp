//
// Created by amirt on 4/3/2024.
//

#include "Arduino.h"

#include "LEDHandler.hpp"

static constexpr auto LED_PIN = 12;
static constexpr auto NUM_LEDS = 30;

LEDHandler<LED_PIN, NUM_LEDS> led_handler;

void setup() {
  led_handler.setBrightness(10);
}

void loop() {
  led_handler.set_n(5, CRGB::Red);
  delay(30);
  led_handler.clear(true);
  delay(30);
}
