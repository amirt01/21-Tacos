//
// Created by amirt on 3/11/2024.
//

#ifndef SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
#define SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_

#include "Arduino.h"

template<int button_pin>
class Button {
  // TODO: measure the optimal debounce_time
  static constexpr auto debounce_time{50};  // [ms]

  // Pin state tracking variables
  int last_pin_state{HIGH};
  enum class ButtonState { Released, Pressed } button_state{};
  unsigned long press_start_time{};

 public:
  Button() {
    pinMode(button_pin, INPUT_PULLUP);
  }

  void loop() {
    const auto pin_state = digitalRead(button_pin);

    if (pin_state == HIGH) {
      button_state = ButtonState::Released;
      press_start_time = 0;
    } else if (const auto current_time = millis(); current_time - press_start_time > debounce_time) {
      if (last_pin_state == HIGH) {
        press_start_time = current_time;
      } else {
        button_state = ButtonState::Pressed;
      }
    }

    last_pin_state = pin_state;
  }

  /** Getters **/
  [[nodiscard]] constexpr bool state() const noexcept { return button_state == ButtonState::Pressed; }
  [[nodiscard]] explicit operator bool() const noexcept { return this->state(); }

  [[nodiscard]] constexpr unsigned long get_press_start_time() const noexcept { return press_start_time; };

  [[maybe_unused, nodiscard]] std::string_view to_str() const noexcept {
    switch(button_state) {
      case ButtonState::Pressed: return "Pressed";
      case ButtonState::Released: return "Released";
      default: return "wtf...";
    }
  }
};

#endif //SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
