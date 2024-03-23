//
// Created by amirt on 3/9/2024.
//

#ifndef SMARTCYCLE_LIB_GROUNDSPEEDESTIMATOR_HPP_
#define SMARTCYCLE_LIB_GROUNDSPEEDESTIMATOR_HPP_

#include <cstdint>
#include <cmath>

#include <utility>

#include "Arduino.h"

template<int reed_switch_pin>
class GroundEstimator {
  /* TIMING */
  unsigned long last_reed_time{};
  unsigned long last_update_time{};

  /* ESTIMATES */
  float spr{infinityf()};  // seconds per revolution
  float speed{};           // [mps]
  float acceleration{};    // [mps2]

  void update_estimates(const unsigned long current_time) {
    /* PHYSICAL CONSTANTS */
    static constexpr float WHEEL_DIAMETER{0.7f};  // [meters] wheels are 700mm
    static constexpr float PI_F{M_PI};

    spr = current_time - last_reed_time;

    // FIXME: divide by zero if spr or is zero
    const auto ds = -(std::exchange(speed, WHEEL_DIAMETER * PI_F / spr) - speed);
    const auto dt = current_time - last_update_time;

    acceleration = ds / dt;
    last_update_time = current_time;
  }

  void ISR() {
    // TODO: find the optimal debounce time
    static constexpr auto DEBOUNCE_TIME{50};

    const auto current_time = millis();
    if (current_time - last_reed_time > DEBOUNCE_TIME) {
      last_reed_time = current_time;
      update_estimates(current_time);
    }
  }

  GroundEstimator() {
    pinMode(reed_switch_pin, INPUT_PULLUP);
  }

 public:
  GroundEstimator(GroundEstimator const&) = delete;
  void operator=(GroundEstimator const&) = delete;

  static GroundEstimator& get_instance() {
    static GroundEstimator ge{};
    return ge;
  }

  void setup() {
    attachInterrupt(digitalPinToInterrupt(reed_switch_pin),
                    [] { GroundEstimator::get_instance().ISR(); },
                    FALLING);
  }

  void loop() {
    const auto current_time = millis();
    if (current_time - last_reed_time > spr) {
      update_estimates(current_time);
    }
  }

  [[nodiscard]] float get_speed() const { return speed; }
  [[nodiscard]] float get_acceleration() const { return acceleration; }
};

#endif //SMARTCYCLE_LIB_GROUNDSPEEDESTIMATOR_HPP_
