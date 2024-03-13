//
// Created by amirt on 3/9/2024.
//

#include <arduino.h>

#include "GroundEstimator.hpp"

static constexpr uint8_t REED_SWITCH_PIN{13};

bool test_flag;

void setup() {
  Serial.begin(115200);

  //TODO: test if INPUT_PULLUP is the best pin mode
  pinMode(REED_SWITCH_PIN, INPUT_PULLUP);
  // TODO: test to see if RISING is the best interrupt mode
  attachInterrupt(digitalPinToInterrupt(REED_SWITCH_PIN), [] {
    GroundEstimator::get_ground_estimator().set_reed_switch_flag();
  }, RISING);
}

void loop() {
  GroundEstimator::get_ground_estimator().update(micros());
  if (test_flag) {
    test_flag = false;
//    Serial.print("hit");
  }
  Serial.printf("%lu\tspeed: %f\tacceleration: %f\n",
                micros(),
                GroundEstimator::get_ground_estimator().get_speed(),
                GroundEstimator::get_ground_estimator().get_acceleration());
}
