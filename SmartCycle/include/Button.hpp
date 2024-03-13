//
// Created by amirt on 3/11/2024.
//

#ifndef SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
#define SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_

#include <string>

#include "Arduino.h"

template<int button_pin>
class Button {
  static constexpr auto debounce_time{50};  // [ms]

  byte last_pin_state{HIGH};
  bool button_state{false};  // false -> released; true -> pressed

  unsigned long press_start_time{};

 public:
  Button() {
    pinMode(button_pin, INPUT_PULLUP);
  }

  void update() {
    const unsigned long current_time = millis();
    const byte pin_state = digitalRead(button_pin);
    const auto dt = current_time - press_start_time;
    if (pin_state == HIGH) {
      button_state = false;
    } else if (dt > debounce_time) {
      if (last_pin_state == HIGH) {
        press_start_time = current_time;
      } else {
        button_state = true;
      }
    }

    last_pin_state = pin_state;
  }

  [[nodiscard]] constexpr bool get_button_state() const noexcept { return button_state; }

  [[maybe_unused, nodiscard]] std::string get_button_state_string() const noexcept {
    if (button_state) {
      return "Pressed";
    } else {
      return "Released";
    }
  }
};

#endif //SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
