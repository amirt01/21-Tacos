//
// Created by amirt on 3/10/2024.
//

#ifndef SMARTCYCLE_INCLUDE_SHIFTER_HPP_
#define SMARTCYCLE_INCLUDE_SHIFTER_HPP_

#include <array>
#include <algorithm>
#include <cmath>

#include "Arduino.h"

class Shifter {
 public:
  /* PHYSICAL CONSTANTS */
  // TODO: find the actual min/max encoder values
  static constexpr uint8_t MIN_GEAR{1};
  static constexpr uint8_t MAX_GEAR{6};

  enum class shift_mode { AUTOMATIC, MANUAL } shift_mode{};

 private:
  // TODO: find the actual nominal encoder values
  std::array<float, MAX_GEAR> nominal_gear_encoder_values{
      100,  // first gear
      200,  // second gear
      300,  // third gear
      400,  // fourth gear
      500,  // fifth gear
      600   // sixth gear
  };

  uint8_t target_gear{1};    // [1-6]
  float motor_signal{};

  /* GEAR ESTIMATION LOGIC */
  float encoder_value{nominal_gear_encoder_values.front()};  // [encoder range] TODD: find encoder range

 public:
  void loop() {
    // PID towards the target gear
    const float encoder_value_error = nominal_gear_encoder_values.at(target_gear - 1) - encoder_value;

    // TODO: figure out optimal Kp, Ki, Kd constants
    static constexpr int Kp{1};
    // TODO: implement integral and derivative terms if necessary
    motor_signal = encoder_value_error * Kp;
  }

  [[nodiscard]] int get_current_gear() const {
    auto closest_nominal_encoder_value_itr =
        std::min_element(nominal_gear_encoder_values.cbegin(), nominal_gear_encoder_values.cend(),
                         [this](float a, float b) { return abs(a - encoder_value) < abs(b - encoder_value); });
    return std::distance(nominal_gear_encoder_values.begin(), closest_nominal_encoder_value_itr) + 1;
  }

  void set_target_gear(const uint8_t gear) { shift(static_cast<shift_direction>(gear - get_current_gear())); }
  [[nodiscard]] int get_target_gear() const { return target_gear; }

  void reset() { target_gear = get_current_gear(); }

  [[nodiscard]] float get_motor_signal() const { return motor_signal; };
  [[nodiscard]] auto& get_nominal_gear_encoder_values_ref() const { return nominal_gear_encoder_values; }

  void set_encoder_value(const short new_encoder_value) { encoder_value = new_encoder_value; }

  enum class shift_direction : int8_t { UP = 1, DOWN = -1 };
  void shift(const shift_direction direction) {
    if (target_gear - get_current_gear() != static_cast<int8_t>(direction)) {
      target_gear = std::clamp(static_cast<uint8_t>(target_gear + static_cast<int8_t>(direction)), MIN_GEAR, MAX_GEAR);
    }
  }
};

#endif //SMARTCYCLE_INCLUDE_SHIFTER_HPP_
