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
  LEDHandler() { addLeds<WS2813, LED_PIN, GRB>(leds.data(), NUM_LEDS); }

  void set(const std::size_t led_index, const CRGB color) {
    leds.at(led_index) = color;
    show();
  }

  void set_n(const std::size_t n_leds, const std::size_t offset, const CRGB color) {
    std::fill_n(std::next(std::begin(leds), offset), n_leds, color);
    show();
  }

  void set_n(const std::size_t n_leds, const CRGB color) { set_n(n_leds, 0, color); }
};

#endif //SMARTCYCLE_INCLUDE_LEDHANDLER_HPP_
