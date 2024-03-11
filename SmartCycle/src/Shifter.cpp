#include "Shifter.hpp"

bool Shifter::shift(const int new_gear) {
  target_gear = std::clamp(new_gear, MIN_GEAR, MAX_GEAR);
  return target_gear == new_gear;
}

void Shifter::update() {
  // PID towards the target gear 
  const int encoder_value_error = nominal_gear_encoder_values.at(target_gear - 1) - encoder_value;

  // TODO: figure out optimal Kp, Ki, Kd constants
  static constexpr int Kp{1};
  // TODO: implement integral and derivative terms if necessary
  motor_signal = encoder_value_error * Kp;
}

int Shifter::current_gear() const {
  std::array<int, nominal_gear_encoder_values.size()> distances_to_encoder_value{};
  std::transform(nominal_gear_encoder_values.cbegin(),
                 nominal_gear_encoder_values.cend(),
                 distances_to_encoder_value.begin(),
                 [this](const int a) { return abs(a - encoder_value); });

  auto closest_nominal_encoder_value_itr =
      std::min_element(distances_to_encoder_value.cbegin(), distances_to_encoder_value.cend());

  return std::distance(std::begin(nominal_gear_encoder_values), closest_nominal_encoder_value_itr) + 1;
};
