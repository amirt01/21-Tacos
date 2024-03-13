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

  GroundEstimator() = default;

 public:
  GroundEstimator(GroundEstimator const&) = delete;
  void operator=(GroundEstimator const&) = delete;

  static GroundEstimator& get_ground_estimator() {
    static GroundEstimator ge;
    return ge;
  }

  void update(unsigned long current_time);

  constexpr void set_reed_switch_flag(const bool value = true) noexcept { reed_switch_flag = value; }

  [[nodiscard]] float get_speed() const { return speed; }
  [[nodiscard]] float get_acceleration() const { return acceleration; }
};

#endif //SMARTCYCLE_LIB_GROUNDSPEEDESTIMATOR_HPP_
