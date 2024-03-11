#include "GroundEstimator.hpp"

void GroundEstimator::update(const unsigned long current_time) {
  /* PHYSICAL CONSTANTS */
  static constexpr float WHEEL_DIAMETER{0.7f};  // [meters] wheels are 700mm
  static constexpr float PI_F{M_PI};
  static constexpr float us_to_s{1e-6f};

  const auto time_since_last_reed = float(current_time - last_reed_time) * us_to_s;  // [s]

  auto update_estimates = [this, current_time, time_since_last_reed] {
    spr = time_since_last_reed;

    // FIXME: divide by zero if spr or is zero
    const auto ds = -(std::exchange(speed, WHEEL_DIAMETER * PI_F / spr) - speed);
    const auto dt = float(current_time - last_update_time) * us_to_s;

    acceleration = ds / dt;
    last_update_time = current_time;
  };

  // TODO: look for a cleaner logic flow
  if (!reed_switch_flag) {
    if (time_since_last_reed > spr) {
      update_estimates();
    }
    // TODO: find the optimal debounce time
  } else {
    reed_switch_flag = false;
    if (static constexpr float DEBOUNCE_TIME{0.1f}; time_since_last_reed > DEBOUNCE_TIME) {
      last_reed_time = current_time;
      update_estimates();
    }
  }
}
