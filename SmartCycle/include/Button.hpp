//
// Created by amirt on 3/11/2024.
//

#ifndef SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
#define SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_

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
    const byte pin_state = digitalRead(button_pin);

    if (pin_state == HIGH) {
      button_state = false;
    } else if (const auto current_time = millis(); current_time - press_start_time > debounce_time) {
      if (last_pin_state == HIGH) {
        press_start_time = current_time;
      } else {
        button_state = true;
      }
    }

    last_pin_state = pin_state;
  }

  [[nodiscard]] explicit operator bool() const noexcept { return button_state; }

  [[nodiscard]] constexpr bool state() const noexcept { return button_state; }

  [[nodiscard]] constexpr unsigned long get_press_start_time() const noexcept { return press_start_time; };

  [[maybe_unused, nodiscard]] std::string_view to_str() const noexcept {
    return button_state ? "Pressed" : "Released";
  }
};

#endif //SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
