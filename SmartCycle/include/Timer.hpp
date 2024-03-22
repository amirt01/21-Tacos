//
// Created by amirt on 3/21/2024.
//

#ifndef SMARTCYCLE_INCLUDE_TIMER_HPP_
#define SMARTCYCLE_INCLUDE_TIMER_HPP_

#include "Arduino.h"

class Timer {
  unsigned long last_pub_time{};
  unsigned long interval;
  void (* callback)();

 public:
  Timer(unsigned long interval, void(* callback)()) : interval(interval), callback(callback) {}

  void loop() {
    if (auto current_time = millis(); current_time - last_pub_time > interval) {
      last_pub_time = current_time;
      callback();
    }
  }
};

#endif //SMARTCYCLE_INCLUDE_TIMER_HPP_
