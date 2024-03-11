//
// Created by amirt on 3/9/2024.
//

#ifndef SMARTCYCLE_LIB_GROUNDSPEEDESTIMATOR_HPP_
#define SMARTCYCLE_LIB_GROUNDSPEEDESTIMATOR_HPP_

#include <cstdint>
#include <cmath>

#include <utility>

class GroundEstimator {
  /* Flags */
  bool reed_switch_flag{};

  /* TIMING */
  unsigned long last_reed_time{};
  unsigned long last_update_time{};

  /* ESTIMATES */
  float spr{infinityf()};  // seconds per revolution
  float speed{};           // [mps]
  float acceleration{};    // [mps2]

 public:
  void update(unsigned long current_time);

  bool set_reed_switch_flag(const bool value = true) noexcept { return reed_switch_flag = value; }

  [[nodiscard]] float get_speed() const { return speed; }
  [[nodiscard]] float get_acceleration() const { return acceleration; }
};

#endif //SMARTCYCLE_LIB_GROUNDSPEEDESTIMATOR_HPP_
