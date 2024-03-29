#include <Arduino.h>
#include "esp_now.h"

#include "Shifter.hpp"
#include "GroundEstimator.hpp"
#include "ButtonHandler.hpp"
#include "SmartCycleServer.hpp"

/** PINS **/
static constexpr uint8_t REED_SWITCH_PIN{27};
static constexpr uint8_t UP_SHIFT_BUTTON_PIN{12};
static constexpr uint8_t DOWN_SHIFT_BUTTON_PIN{13};
static constexpr uint8_t MOTOR_PIN{18};

/** STATES **/
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

/** Server **/
auto& server = SmartCycleServer::get_instance();
void update_server_values();

/** CURRENT ESTIMATES **/
auto& ground_estimator = GroundEstimator<REED_SWITCH_PIN>::get_instance();
float cadence{};

/** SHIFTING **/
Shifter shifter{};
auto& up_shift_button = ButtonHandler<UP_SHIFT_BUTTON_PIN>::get_instance();
auto& down_shift_button = ButtonHandler<DOWN_SHIFT_BUTTON_PIN>::get_instance();

/** RUN FUNCTIONS **/
[[maybe_unused]] void log();

[[maybe_unused]] bool safe_to_shift();

void setup() {
  Serial.begin(115200);

  /** Setup Wireless Communications **/
  WiFiClass::mode(WIFI_AP_STA);

  server.setup();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb([](const uint8_t*, const uint8_t* data, int data_len) { memcpy(&cadence, data, data_len); });

  /** Setup Ground Estimator **/
  ground_estimator.setup();

  /** Button Setup **/
  up_shift_button.setup();
  down_shift_button.setup();
}

void loop() {
  ground_estimator.loop();
  update_server_values();
  server.loop();

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

      shifter.loop();
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

      shifter.loop();
    }
  }
}

void update_server_values() {
  server.set("speed", ground_estimator.get_speed());
  server.set("cadence", cadence);
  server.set("target gear", shifter.get_target_gear());
  server.set("current gear", shifter.current_gear());
  server.set("is_pressed", state_str());
  server.set("up shift button", up_shift_button.status_str());
  server.set("down shift button", down_shift_button.status_str());
}

void log() {
  Serial.printf("State: %s\tUp Button: %s\tDown Button: %s\tSpeed: %f\tCadence: %f\tCurrent Gear: %i\t"
                "Target Gear: %i\n",
                state_str().data(),
                up_shift_button.status_str().data(), down_shift_button.status_str().data(),
                ground_estimator.get_speed(), cadence,
                shifter.current_gear(), shifter.get_target_gear());
}

bool safe_to_shift() {
  // TODO: find the value to enable shifting
  static constexpr auto SHIFT_PEDAL_CADENCE_CUTOFF{0.5f};  // [rpm]
  return cadence < SHIFT_PEDAL_CADENCE_CUTOFF;  // only shift while pedaling
}
