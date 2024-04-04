//
// Created by amirt on 4/3/2024.
//

#ifndef SMARTCYCLE_INCLUDE_LEDHANDLER_HPP_
#define SMARTCYCLE_INCLUDE_LEDHANDLER_HPP_

#include <array>

#include "FastLED.h"

template<uint8_t LED_PIN, std::size_t NUM_LEDS>
class LEDHandler : public CFastLED {
  std::array<CRGB, NUM_LEDS> leds;

 public:
  LEDHandler() {
    addLeds<WS2813, LED_PIN, GRB>(leds.data(), NUM_LEDS);
    clear(true);
  }

  void set(const std::size_t led_index, const CRGB color) {
    leds.at(led_index) = color;
  }

  void set_n(const std::size_t n_leds, const CRGB color) {
    std::fill_n(std::begin(leds), n_leds, color);
  }
};

#endif //SMARTCYCLE_INCLUDE_LEDHANDLER_HPP_
