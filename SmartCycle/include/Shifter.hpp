//
// Created by amirt on 3/10/2024.
//

#ifndef SMARTCYCLE_INCLUDE_SHIFTER_HPP_
#define SMARTCYCLE_INCLUDE_SHIFTER_HPP_

#include <array>
#include <algorithm>

class Shifter {
  /* PHYSICAL CONSTANTS */
  // TODO: find the actual min/max encoder values
  static constexpr int MIN_GEAR{1};
  static constexpr int MAX_GEAR{6};

  // TODO: find the actual nominal encoder values
  static constexpr std::array<int, MAX_GEAR> nominal_gear_encoder_values{
      100,  // first gear
      200,  // second gear
      300,  // third gear
      400,  // fourth gear
      500,  // fifth gear
      600   // sixth gear
  };

  int target_gear{1};    // [1-6]
  int motor_signal{};

  /* ESTIMATES */
  short encoder_value{};  // [encoder range] TODD: find encoder range

  constexpr bool shift(int new_gear) {
    target_gear = std::clamp(new_gear, MIN_GEAR, MAX_GEAR);
    return target_gear == new_gear;
  }

 public:
  void update() {
    // PID towards the target gear
    const int encoder_value_error = nominal_gear_encoder_values.at(target_gear - 1) - encoder_value;

    // TODO: figure out optimal Kp, Ki, Kd constants
    static constexpr int Kp{1};
    // TODO: implement integral and derivative terms if necessary
    motor_signal = encoder_value_error * Kp;
  }

  [[nodiscard]] int current_gear() const {
    std::array<int, nominal_gear_encoder_values.size()> distances_to_encoder_value{};
    std::transform(nominal_gear_encoder_values.cbegin(),
                   nominal_gear_encoder_values.cend(),
                   distances_to_encoder_value.begin(),
                   [this](const int a) { return abs(a - encoder_value); });

    auto closest_nominal_encoder_value_itr =
        std::min_element(distances_to_encoder_value.cbegin(), distances_to_encoder_value.cend());

    return std::distance(distances_to_encoder_value.cbegin(), closest_nominal_encoder_value_itr) + 1;
  }

  [[nodiscard]] int get_motor_signal() const { return motor_signal; };

  void set_encoder_value(const short new_encoder_value) { encoder_value = new_encoder_value; }

  // Return if shift successful
  constexpr bool shift_up() { return shift(target_gear + 1); }
  constexpr bool shift_down() { return shift(target_gear - 1); }
};

#endif //SMARTCYCLE_INCLUDE_SHIFTER_HPP_
