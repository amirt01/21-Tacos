//
// Created by amirt on 3/11/2024.
//

#ifndef SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
#define SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_

#include <Arduino.h>

template<int button_pin, uint8_t timer>
class ButtonHandler {
  // Debounce timer calculations and initialization
  // TODO: measure the optimal debounce_time
  static constexpr auto debounce_time = 50;  // [ms]
  static constexpr auto divider = 80;  // [Hz]
  static constexpr auto alarm_value = debounce_time * 1000;  // [us]
  hw_timer_s* debounce_timer_config = timerBegin(timer, divider, true);

  // Pin state tracking variables
  volatile enum class ButtonState : bool { Released, Pressed } button_state{};

  void IRAM_ATTR button_ISR() {
    timerWrite(ButtonHandler::get_instance().debounce_timer_config, 0);
    timerAlarmEnable(debounce_timer_config);
  }

  void IRAM_ATTR debounce_ISR() {
    ButtonHandler::get_instance().button_state = static_cast<ButtonState>(!digitalRead(button_pin));
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
    attachInterrupt(digitalPinToInterrupt(button_pin),
                    [] { ButtonHandler::get_instance().button_ISR(); },
                    CHANGE);

    // Setup debounce timer
    timerAttachInterrupt(
        debounce_timer_config,
        [] { ButtonHandler::get_instance().debounce_ISR(); },
        true
    );
    timerAlarmWrite(debounce_timer_config, alarm_value, false);
  }

  /** Getters **/
  [[nodiscard]] constexpr bool state() const noexcept { return button_state == ButtonState::Pressed; }
  [[nodiscard]] explicit operator bool() const noexcept { return this->state(); }

  [[maybe_unused, nodiscard]] std::string_view to_str() const noexcept {
    switch (button_state) {
      case ButtonState::Pressed: return "Pressed";
      case ButtonState::Released: return "Released";
      default: return "wtf...";
    }
  }
};

#endif //SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
