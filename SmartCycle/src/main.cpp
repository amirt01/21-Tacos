#include <Arduino.h>
#include "esp_now.h"

#include "Shifter.hpp"
#include "GroundEstimator.hpp"
#include "Button.hpp"
#include "SmartCycleServer.hpp"

/* PINS */
static constexpr uint8_t REED_SWITCH_PIN{27};
static constexpr uint8_t UP_SHIFT_BUTTON_PIN{12};
static constexpr uint8_t DOWN_SHIFT_BUTTON_PIN{13};
static constexpr uint8_t MOTOR_PIN{18};

/* STATES */
enum class States {
  Asleep,            // Low power mode
  Stopped,           // Waiting to bike again
  Biking             // Coasting or Pedaling at constant speed
} current_state;

std::string_view state_str() {
  switch (current_state) {
    case States::Stopped: return "Stopped";
    case States::Biking: return "Biking";
    case States::Asleep: return "Asleep";
    default: return "wtf...";
  }
}

/* Server */
SmartCycleServer server{};

/* CURRENT ESTIMATES */
auto& ground_estimator = GroundEstimator<REED_SWITCH_PIN>::get_ground_estimator();    // [meters per second]

/* SHIFTING */
Shifter shifter{};
Button<UP_SHIFT_BUTTON_PIN> up_shift_button{};
Button<DOWN_SHIFT_BUTTON_PIN> down_shift_button{};

/* RUN FUNCTIONS */
void update_anticipations() {
  // TODO: implement this
  // anticipate using the camera (i.e. slowing down via stop sign, traffic light, object on road, etc)
}

// Message Structure for Cadence
float cadence;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
  if (len == sizeof(cadence)) {
    memcpy(&cadence, incomingData, len);
  } else {
    Serial.printf("ERROR: Package is size: %i, but should be size: %i\n", len, sizeof(cadence));
  }
}

void update_server_values();

[[maybe_unused]] void log();

[[maybe_unused]] bool safe_to_shift();

void setup() {
  Serial.begin(115200);

  server.setup();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB 
  esp_now_register_recv_cb(OnDataRecv);

  ground_estimator.setup();
}

void loop() {
  up_shift_button.update();
  down_shift_button.update();
  ground_estimator.update();
  update_server_values();
  static unsigned long last_pub_time{0};
  if (millis() - last_pub_time > 500) {
    last_pub_time = millis();
    server.update();
  }

  switch (current_state) {
    case States::Asleep: {
      if (up_shift_button || down_shift_button) {
        current_state = States::Stopped;
      } else if (ground_estimator.get_speed() > 0.1) {
        current_state = States::Biking;
      }
      break;
    }
    case States::Stopped: {
      if (ground_estimator.get_speed() > 0.1) {
        current_state = States::Biking;
      }

      shifter.update();
      break;
    }
    case States::Biking: {
      // TODO: find the stop speed cutoff value
      static constexpr auto STOP_SPEED_CUTOFF{0.1};  // [meters per second]
      if (ground_estimator.get_speed() < STOP_SPEED_CUTOFF) {
        shifter.reset();
        current_state = States::Stopped;
        break;
      }

      // TODO: find the value and implement acceleration shifting
      static constexpr auto SHIFT_ACCELERATION_CUTOFF{0.5f};   // [meters per second per second]
      if (up_shift_button) {
        shifter.shift_up();
      } else if (down_shift_button) {
        shifter.shift_down();
      }
    }
  }
}

void update_server_values() {
  auto update_if_new = [](std::string_view key, auto value) {
    if (server.get<decltype(value)>(key) != value) {
      server.set(key, value);
    } else {
      //TODO: remove old values
    }
  };

  update_if_new("speed", ground_estimator.get_speed());
  update_if_new("cadence", cadence);
  update_if_new("target gear", shifter.get_target_gear());
  update_if_new("current gear", shifter.current_gear());
  update_if_new("state", state_str());
  update_if_new("up shift button", up_shift_button.to_str());
  update_if_new("down shift button", down_shift_button.to_str());
}

void log() {
  Serial.printf("State: %s\tUp Button: %s\tDown Button: %s\tSpeed: %f\tCadence: %f\tCurrent Gear: %i\t"
                "Target Gear: %i\n",
                state_str().data(),
                up_shift_button.to_str().data(), down_shift_button.to_str().data(),
                ground_estimator.get_speed(), cadence, shifter.current_gear(),
                shifter.get_target_gear());
}

bool safe_to_shift() {
  // TODO: find the value to enable shifting
  static constexpr auto SHIFT_PEDAL_CADENCE_CUTOFF{0.5f};  // [rpm]
  return cadence < SHIFT_PEDAL_CADENCE_CUTOFF;  // only shift while pedaling
}
