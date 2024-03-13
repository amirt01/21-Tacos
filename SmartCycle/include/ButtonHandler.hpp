//
// Created by amirt on 3/11/2024.
//

#ifndef SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
#define SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_

#include <string>

#include "Arduino.h"

enum class ButtonState {
  Released,
  Pressed,
  Held
};

template<int button_pin>
class ButtonHandler {
  static constexpr auto debounce_time{50};  // [ms]
  static constexpr auto hold_time{500};    // [ms]

  byte last_pin_state{HIGH};
  ButtonState button_state{ButtonState::Released};

  unsigned long press_start_time{};

 public:
  ButtonHandler() {
    pinMode(button_pin, INPUT_PULLUP);
  }

  void update() {
    const unsigned long current_time = millis();
    const byte pin_state = digitalRead(button_pin);
    const auto dt = current_time - press_start_time;
    if (pin_state == HIGH) {
      button_state = ButtonState::Released;
    } else if (dt > debounce_time) {
      if (last_pin_state == HIGH) {
        press_start_time = current_time;
      } else if (dt > hold_time) {
        button_state = ButtonState::Held;
      } else {
        button_state = ButtonState::Pressed;
      }
    }

    last_pin_state = pin_state;
  }

  [[nodiscard]] constexpr ButtonState get_button_state() const noexcept { return button_state; }

  [[maybe_unused, nodiscard]] std::string get_button_state_string() const noexcept {
    switch (button_state) {
      case ButtonState::Pressed: return "Pressed";
      case ButtonState::Released: return "Released";
      case ButtonState::Held: return "Held";
      default: return "wtf...";
    }
  }
};

#endif //SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
