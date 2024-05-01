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
  union TuningUnion {
    Tuning tuning_msg;
    decltype(Tuning::nominal_gear_encoder_value_1) encoder_values[MAX_GEAR];
  };

  TuningUnion tuning_values{
      .tuning_msg{
          .nominal_gear_encoder_value_1 = 100,
          .nominal_gear_encoder_value_2 = 200,
          .nominal_gear_encoder_value_3 = 300,
          .nominal_gear_encoder_value_4 = 400,
          .nominal_gear_encoder_value_5 = 500,
          .nominal_gear_encoder_value_6 = 600,
          .desired_cadence_high = 90,
          .desired_cadence_low = 60
      }
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

  static constexpr float controller_dt_us = 0.1e3;  // [us]
  esp_timer_handle_t controller_timer{};
  esp_timer_create_args_t controller_timer_args{
      [](void* handler_ptr) { static_cast<Shifter*>(handler_ptr)->controller_ISR(); },
      this,
      ESP_TIMER_TASK,
      "controller timer",
      false
  };

  // PID towards the target gear
  void controller_ISR() {
    static constexpr auto const_dt_ms = controller_dt_us / 1000.f;  // [s]
    static int64_t encoder_value_error{};

    const auto last_error = encoder_value_error;
    encoder_value_error = tuning_values.encoder_values[target_gear - 1] - encoder.getCount();

    // Our sensors are bad so just assume 1 deg of error is 0
    static constexpr auto buffer = 1;
    if (abs(encoder_value_error) < buffer) {
      encoder_value_error = 0;
    }

    // TODO: figure out optimal Kp, Ki, Kd constants
    static constexpr float Kp{2}, Ki{}, Kd{};

    const auto P_term = Kp * encoder_value_error;
//    const auto D_term = Kd * (encoder_value_error - last_error) / const_dt_ms;  // real-time!

    static constexpr int32_t motor_speed_offset = 256;
    static constexpr int32_t motor_speed_max = 1023;

    const int32_t motor_speed = std::clamp((int32_t)(P_term), -motor_speed_max, motor_speed_max);
    const auto motor_command = abs(motor_speed) + motor_speed_offset;

    ledcWrite(R_PWM_CHAN, (motor_speed > 0) ? 0 : motor_command);
    ledcWrite(L_PWM_CHAN, (motor_speed < 0) ? 0 : motor_command);
  }

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

    // motor setup
    pinMode(motor_pin_R, OUTPUT);
    pinMode(motor_pin_L, OUTPUT);

    ledcSetup(R_PWM_CHAN, 1000, 10);
    ledcAttachPin(motor_pin_R, R_PWM_CHAN);

    ledcSetup(L_PWM_CHAN, 1000, 10);
    ledcAttachPin(motor_pin_L, L_PWM_CHAN);

    // timer setup
    esp_timer_create(&timer_args, &shift_cooldown_timer);
    esp_timer_create(&controller_timer_args, &controller_timer);
  }

  [[nodiscard]] int get_current_gear() {
    auto closest_nominal_encoder_value_itr =
        std::min_element(std::begin(tuning_values.encoder_values), std::end(tuning_values.encoder_values),
                         [this](float a, float b) {
                           return abs(a - encoder.getCount()) < abs(b - encoder.getCount());
                         });
    return std::distance(std::begin(tuning_values.encoder_values), closest_nominal_encoder_value_itr) + 1;
  }

  void set_target_gear(const uint8_t gear) { shift(static_cast<shift_direction>(gear - get_current_gear())); }
  [[nodiscard]] int get_target_gear() const { return target_gear; }
  [[nodiscard]] Tuning* get_tuning_ptr() { return &tuning_values.tuning_msg; }

  [[nodiscard]] bool in_cadence_range(const float cadence) const {
    return tuning_values.tuning_msg.desired_cadence_low < cadence
        && cadence < tuning_values.tuning_msg.desired_cadence_high;
  }

  void reset() {
    disable_shifting();
    target_gear = get_current_gear();
  }

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

  void enable_shifting() {
    Serial.println("shifting enabled");
    esp_timer_start_periodic(controller_timer, controller_dt_us);
  };
  void disable_shifting() { esp_timer_stop(controller_timer); };
};

#endif //SMARTCYCLE_INCLUDE_SHIFTER_HPP_
