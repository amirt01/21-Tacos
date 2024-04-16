//
// Created by amirt on 3/11/2024.
//

#ifndef SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_
#define SMARTCYCLE_LIB_BUTTON_BUTTON_HPP_

#include <Arduino.h>

template<int button_pin>
class ButtonHandler {
  // State tracking variable
  volatile enum class ButtonState : bool { Released, Pressed } button_state{};

  // Start the debounce timer when the button is pressed
  void IRAM_ATTR button_ISR() {
    if (!esp_timer_is_active(debounce_timer)) {
      esp_timer_start_once(debounce_timer, debounce_time);
    }
  }

  // Update the button state with the pin value when the debounce alarm goes off
  void debounce_ISR(ButtonHandler* handler) {
    handler->button_state = static_cast<ButtonState>(!digitalRead(button_pin));
  }

  // Debounce timer initialization
  // TODO: measure the optimal debounce_time
  static constexpr auto debounce_time = 5e4;  // [us]
  esp_timer_handle_t debounce_timer{};
  esp_timer_create_args_t timer_args{
      [](void* handler_ptr) {
        auto handler = static_cast<ButtonHandler*>(handler_ptr);
        handler->debounce_ISR(handler);
      },
      this,
      ESP_TIMER_TASK,
      "button timer",
      false
  };

  ButtonHandler() = default;

 public:
  ButtonHandler(ButtonHandler const&) = delete;
  void operator=(ButtonHandler const&) = delete;

  static ButtonHandler& get_instance() {
    static ButtonHandler bh{};
    return bh;
  }

  void setup() {
    // Setup button interrupt
    pinMode(button_pin, INPUT_PULLUP);
    attachInterrupt(
        digitalPinToInterrupt(button_pin),
        []() IRAM_ATTR { ButtonHandler::get_instance().button_ISR(); },
        CHANGE
    );

    // Setup debounce alarm
    esp_timer_create(&timer_args, &debounce_timer);
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
