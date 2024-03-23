//
// Created by amirt on 3/11/2024.
//

#ifndef SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
#define SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_

#include "Arduino.h"

template<int button_pin>
class ButtonHandler {

  // Pin state tracking variables
  bool button_pressed{};
  unsigned long last_change_time{};

  void ISR() {
    const bool pin_state = digitalRead(button_pin);

    // TODO: measure the optimal debounce_time
    static constexpr auto debounce_time{50};  // [ms]
    const auto current_time = millis();
    if (current_time - last_change_time > debounce_time) {
      button_pressed = !pin_state;
      last_change_time = current_time;
    }
  }

  ButtonHandler() {
    pinMode(button_pin, INPUT_PULLUP);
  }

 public:
  ButtonHandler(ButtonHandler const&) = delete;
  void operator=(ButtonHandler const&) = delete;

  static ButtonHandler& get_instance() {
    static ButtonHandler b{};
    return b;
  }

  void setup() {
    attachInterrupt(digitalPinToInterrupt(button_pin), [] { ButtonHandler::get_instance().ISR(); }, CHANGE);
  }

  /** Getters **/
  [[nodiscard]] constexpr
  bool is_pressed() const noexcept { return button_pressed; }
  [[nodiscard]] explicit operator bool() const noexcept { return this->is_pressed(); }

  [[nodiscard]] constexpr unsigned long get_last_change_time() const noexcept { return last_change_time; };

  [[maybe_unused, nodiscard]] std::string_view status_str() const noexcept {
    return button_pressed ? "Pressed" : "Released";
  }
};

#endif //SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
