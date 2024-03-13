//
// Created by amirt on 3/11/2024.
//

#ifndef SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
#define SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_

enum class ButtonState {
  Released,
  Pressed,
  Held
};

class ButtonHandler {
  static constexpr auto debounce_time{50};  // [ms]
  static constexpr auto hold_time{3000};    // [ms]
  static constexpr auto tap_time{500};      // [ms]

  enum PinState { HIGH, LOW } last_pin_state{HIGH};
  ButtonState button_state{ButtonState::Released};

  int pin_number{};

  int n_taps{};

  unsigned long press_start_time{};

 public:
  void update(const unsigned long current_time, const PinState pin_state) {
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
};

#endif //SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
