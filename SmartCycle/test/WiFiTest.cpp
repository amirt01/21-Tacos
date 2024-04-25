//
// Created by amirt on 3/14/2024.
//

#include "Arduino.h"

#include "SmartCycleServer.hpp"

auto& server = SmartCycleServer::get_instance();

// Handling incoming tuning messages
void handle_tuning_message(const Tuning& tuning_msg) {
  // FOR FUNCTION TESTING
  Serial.printf(
    "Received tuning message:\n[ %i, %i, %i, %i, %i, %i ]\n",
    tuning_msg.nominal_gear_encoder_value_1,
    tuning_msg.nominal_gear_encoder_value_2,
    tuning_msg.nominal_gear_encoder_value_3,
    tuning_msg.nominal_gear_encoder_value_4,
    tuning_msg.nominal_gear_encoder_value_5,
    tuning_msg.nominal_gear_encoder_value_6
  );
  Serial.printf("Desired Cadence High: %f\n", tuning_msg.desired_cadence_high);
  Serial.printf("Desired Cadence Low: %f\n", tuning_msg.desired_cadence_low);
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  server.setup();
  server.set_tuning_callback(handle_tuning_message);
}

void loop() {
  server.loop();
}
