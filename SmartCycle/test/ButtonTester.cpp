//
// Created by amirt on 3/13/2024.
//

#include <arduino.h>

#include "ButtonHandler.hpp"

auto& bh = ButtonHandler<13, 1>::get_instance();
auto& bh2 = ButtonHandler<14, 2>::get_instance();

void setup() {
  Serial.begin(115200);

  bh.setup();
  bh2.setup();
}

void loop() {
  Serial.printf("%lu\tbutton state: %s\n", millis(), bh.to_str().data());
}
