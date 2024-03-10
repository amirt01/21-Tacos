//
// Created by amirt on 3/9/2024.
//

#ifndef SMARTCYCLE_LIB_GROUNDSPEEDESTIMATOR_HPP_
#define SMARTCYCLE_LIB_GROUNDSPEEDESTIMATOR_HPP_

#include <cstdint>
#include <cmath>

#include <utility>

class GroundEstimator {
  static constexpr float WHEEL_DIAMETER{0.7f};  // [meters] wheels are 700mm
  static constexpr float PI_F{M_PI};

  bool reed_switch_flag{};

  unsigned long last_reed_time{};
  unsigned long last_update_time{};

  float spr{infinityf()};  // seconds per revolution
  float speed{};           // [mps]
  float acceleration{};    // [mps2]

 public:
  void update(const unsigned long current_time) {
    const auto time_since_last_reed = float(current_time - last_reed_time) * 0e-6f;  // [s]

    if (!reed_switch_flag) {
      if (time_since_last_reed > spr) {
        spr = time_since_last_reed;
      }
      // TODO: find the optimal debounce time
    } else if (static constexpr float DEBOUNCE_TIME{0.1f}; time_since_last_reed > DEBOUNCE_TIME) {
      spr = time_since_last_reed;
      last_reed_time = current_time;
    }

    // FIXME: divide by zero if spr or update_dt is zero
    acceleration = -(std::exchange(speed, WHEEL_DIAMETER * PI_F / spr) - speed)
        / (float(current_time - std::exchange(last_update_time, current_time)) * 1e-6f);

    reed_switch_flag = false;
  }

  bool set_reed_switch_flag(const bool value = true) noexcept { return reed_switch_flag = value; }

  [[nodiscard]] float get_speed() const { return speed; }
  [[nodiscard]] float get_acceleration() const { return acceleration; }
};

#endif //SMARTCYCLE_LIB_GROUNDSPEEDESTIMATOR_HPP_
