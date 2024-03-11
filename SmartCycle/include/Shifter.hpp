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
  static constexpr std::array<int, MAX_GEAR> nominal_gear_encoder_values {
      100,  // first gear
      200,  // second gear
      300,  // third gear
      400,  // fourth gear
      500,  // fifth gear
      600   // sixth gear
  };

  int target_gear{3};    // [1-6]
  int motor_signal{};

  /* ESTIMATES */
  short encoder_value{};  // [encoder range] TODD: find encoder range

  bool shift(int new_gear);

 public:
  void update();

  [[nodiscard]] int current_gear() const;
  [[nodiscard]] int get_motor_signal() const { return motor_signal; };

  void set_encoder_value(const short new_encoder_value) { encoder_value = new_encoder_value; }

  // Return if shift successful
  bool shift_up() { return shift(target_gear + 1); }
  bool shift_down() { return shift(target_gear - 1); }
};

#endif //SMARTCYCLE_INCLUDE_SHIFTER_HPP_
