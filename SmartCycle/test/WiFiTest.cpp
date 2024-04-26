//
// Created by amirt on 3/14/2024.
//

#include "Arduino.h"

#include "SmartCycleServer.hpp"

auto& server = SmartCycleServer::get_instance();
Tuning tuning_msg = {
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8
};

// Handling incoming tuning messages
void handle_tuning_message() {
  // FOR FUNCTION TESTING
  Serial.printf(
      "Received tuning message:\n[ %lli, %lli, %lli, %lli, %lli, %lli ]\n",
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
  server.set_tuning_msg(&tuning_msg);
  server.set_telemetry_msg({
                               3,
                               4,
                               5,
                               6,
                               Telemetry_State::Telemetry_State_BIKING,
                               Telemetry_ButtonState::Telemetry_ButtonState_RELEASED,
                               Telemetry_ButtonState::Telemetry_ButtonState_PRESSED
                           });
}

void loop() {
  server.loop();

  const auto current_time = millis();
  static auto last_send_time = current_time;
  if (current_time - last_send_time > 500) {
    handle_tuning_message();
    last_send_time = current_time;
  }
}
