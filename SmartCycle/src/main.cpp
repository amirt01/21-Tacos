#include <Arduino.h>

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
  switch(current_state) {
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
float pedal_cadence{};   // [rpm]

/* SHIFTING */
Shifter shifter{};
Button<UP_SHIFT_BUTTON_PIN> up_shift_button{};
Button<DOWN_SHIFT_BUTTON_PIN> down_shift_button{};

/* RUN FUNCTIONS */
void update_anticipations() {
  // TODO: implement this
  // anticipate using the camera (i.e. slowing down via stop sign, traffic light, object on road, etc)
}

void update_server_values();

void log();

[[maybe_unused]] bool safe_to_shift();

void setup() {
  Serial.begin(115200);

  server.setup();
  ground_estimator.setup();
}

void loop() {
  up_shift_button.update();
  down_shift_button.update();
  ground_estimator.update();
  update_server_values();
  static unsigned long last_pub_time{0};
  if (millis() - last_pub_time > 250) {
    last_pub_time = millis();
    server.update();
    log();
  }

  switch (current_state) {
    case States::Asleep: {
//      Serial.println("Asleep");
      if (up_shift_button || down_shift_button) {
        current_state = States::Stopped;
      } else if (ground_estimator.get_speed() > 0.1) {
        current_state = States::Biking;
      }
      break;
    }
    case States::Stopped: {
//      Serial.println("Stopped");
      if (ground_estimator.get_speed() > 0.1) {
        current_state = States::Biking;
      }

      shifter.update();
      break;
    }
    case States::Biking: {
//      Serial.println("Biking");
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
  server.set("speed", ground_estimator.get_speed());
  server.set("cadence", pedal_cadence);
  server.set("target gear", shifter.get_target_gear());
  server.set("current gear", shifter.current_gear());
  server.set("state", state_str());
  server.set("up shift button", up_shift_button.to_str());
  server.set("down shift button", down_shift_button.to_str());
}

void log() {
  Serial.printf("State: %s\tUp Button: %s\tDown Button: %s\tSpeed: %f\tCadence: %f\tCurrent Gear: %i\t"
                "Target Gear: %i\n",
                state_str().data(),
                up_shift_button.to_str().data(), down_shift_button.to_str().data(),
                ground_estimator.get_speed(), pedal_cadence, shifter.current_gear(),
                shifter.get_target_gear());
}

bool safe_to_shift() {
  // TODO: find the value to enable shifting
  static constexpr auto SHIFT_PEDAL_CADENCE_CUTOFF{0.5f};  // [rpm]
  return pedal_cadence < SHIFT_PEDAL_CADENCE_CUTOFF;  // only shift while pedaling
}
