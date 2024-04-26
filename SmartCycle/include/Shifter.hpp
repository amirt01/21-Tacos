//
// Created by amirt on 3/10/2024.
//

#ifndef SMARTCYCLE_INCLUDE_SHIFTER_HPP_
#define SMARTCYCLE_INCLUDE_SHIFTER_HPP_

#include <array>
#include <algorithm>
#include <cmath>
#include <ESP32Encoder.h>
#include <SmartCycle.pb.h>

#include "Arduino.h"
#include "nvs_flash.h"
#include "nvs.h"

class Shifter {
 public:
  /* PINS */
  const int encoder_pin_A;
  const int encoder_pin_B;
  const int motor_pin_R;
  const int motor_pin_L;

  /* PHYSICAL CONSTANTS */
  // TODO: find the actual min/max encoder values
  static constexpr uint8_t MIN_GEAR{1};
  static constexpr uint8_t MAX_GEAR{6};

  static constexpr uint8_t R_PWM_CHAN{0};
  static constexpr uint8_t L_PWM_CHAN{1};

  enum class ShiftMode { AUTOMATIC, MANUAL } shift_mode{};

 private:
  // TODO: find the actual nominal encoder values
  Tuning tuning_values{
      .nominal_gear_encoder_value_1 = 100,
      .nominal_gear_encoder_value_2 = 200,
      .nominal_gear_encoder_value_3 = 300,
      .nominal_gear_encoder_value_4 = 400,
      .nominal_gear_encoder_value_5 = 500,
      .nominal_gear_encoder_value_6 = 600,
      .desired_cadence_high = 90.f,
      .desired_cadence_low = 60.f
  };

  uint8_t target_gear{1};    // [1-6]

  ESP32Encoder encoder;

  static constexpr auto shift_cooldown = 0.5e6;  // [us]
  esp_timer_handle_t shift_cooldown_timer{};
  esp_timer_create_args_t timer_args{
      [](void* handler_ptr) {},
      nullptr,
      ESP_TIMER_TASK,
      "shift cooldown timer",
      false
  };

 public:
  Shifter(int encoder_pin_A, int encoder_pin_B, int motor_pin_R, int motor_pin_L)
      : encoder_pin_A(encoder_pin_A),
        encoder_pin_B(encoder_pin_B),
        motor_pin_R(motor_pin_R),
        motor_pin_L(motor_pin_L) {}

  void setup() {
    // encoder setup
    encoder.attachHalfQuad(encoder_pin_A, encoder_pin_B);
    encoder.setCount(0);

    pinMode(motor_pin_R, OUTPUT);
    pinMode(motor_pin_L, OUTPUT);

    ledcSetup(R_PWM_CHAN, 1000, 10);
    ledcAttachPin(motor_pin_R, R_PWM_CHAN);

    ledcSetup(L_PWM_CHAN, 1000, 10);
    ledcAttachPin(motor_pin_L, L_PWM_CHAN);

    // motor setup

    // timer setup
    esp_timer_create(&timer_args, &shift_cooldown_timer);
  }

  void loop() {
    // PID towards the target gear
    const int64_t encoder_value_error = 1;
//    const int64_t encoder_value_error = nominal_gear_encoder_values.at(target_gear - 1) - encoder.getCount();

    // TODO: figure out optimal Kp, Ki, Kd constants
    static constexpr float Kp{2}, Ki{}, Kd{};

    static constexpr int buffer = 1;
    static constexpr int motor_speed_offset = 128;
    static constexpr int64_t motor_speed_max = 1023;
    const int motor_speed = abs(encoder_value_error) < buffer ? 0 : std::min(
        static_cast<int64_t>(Kp * abs(encoder_value_error)) + motor_speed_offset, motor_speed_max);

    ledcWrite(R_PWM_CHAN, (encoder_value_error > 0) ? 0 : abs(motor_speed));
    ledcWrite(L_PWM_CHAN, (encoder_value_error < 0) ? 0 : abs(motor_speed));
  }

  [[nodiscard]] int get_current_gear() {
    return 1;
//    auto closest_nominal_encoder_value_itr =
//        std::min_element(nominal_gear_encoder_values.cbegin(), nominal_gear_encoder_values.cend(),
//                         [this](float a, float b) { return abs(a - encoder.getCount()) < abs(b - encoder.getCount()); });
//    return std::distance(nominal_gear_encoder_values.cbegin(), closest_nominal_encoder_value_itr) + 1;
  }

  void set_target_gear(const uint8_t gear) { shift(static_cast<shift_direction>(gear - get_current_gear())); }
  [[nodiscard]] int get_target_gear() const { return target_gear; }
  [[nodiscard]] Tuning* get_tuning_ptr() { return &tuning_values; }

  [[nodiscard]] int64_t get_current_encoder_value() { return encoder.getCount(); }

  void reset() { target_gear = get_current_gear(); }

  enum class shift_direction : int8_t { UP = 1, DOWN = -1 };
  void shift(const shift_direction direction) {
    if (esp_timer_is_active(shift_cooldown_timer)) {
      return;
    }

    if (target_gear - get_current_gear() != static_cast<int8_t>(direction)) {
      target_gear = std::clamp(static_cast<uint8_t>(target_gear + static_cast<int8_t>(direction)), MIN_GEAR, MAX_GEAR);
      esp_timer_start_once(shift_cooldown_timer, shift_cooldown);
    }
  }
};

#endif //SMARTCYCLE_INCLUDE_SHIFTER_HPP_
