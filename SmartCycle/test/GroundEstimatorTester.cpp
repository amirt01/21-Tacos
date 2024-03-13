//
// Created by amirt on 3/9/2024.
//

#include <arduino.h>

#include "GroundEstimator.hpp"

static constexpr uint8_t REED_SWITCH_PIN{13};

auto& ge = GroundEstimator<REED_SWITCH_PIN>::get_ground_estimator();

void setup() {
  Serial.begin(115200);

  ge.setup();
}

void loop() {
  ge.update(micros());
  Serial.printf("%lu\tspeed: %f\tacceleration: %f\n",
                micros(),
                ge.get_speed(),
                ge.get_acceleration());
}
