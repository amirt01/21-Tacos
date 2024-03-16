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
} current_state = States::Asleep;

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
  server.update();

  log();

  // FIXME: why does the state not switch from Asleep?!
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

      // TODO: find the value
      static constexpr auto SHIFT_ACCELERATION_CUTOFF{0.5f};   // [meters per second per second]
      if (up_shift_button || ground_estimator.get_acceleration() > SHIFT_ACCELERATION_CUTOFF) {
        shifter.shift_up();
      } else if (down_shift_button || ground_estimator.get_acceleration() < -SHIFT_ACCELERATION_CUTOFF) {
        shifter.shift_down();
      }

      update_anticipations();
      shifter.update();
    }
  }
}

void update_server_values() {
  server.set("speed", ground_estimator.get_speed());
  server.set("cadence", pedal_cadence);
  server.set("gear", shifter.current_gear());
}

void log() {
  std::string_view state_string;
  switch(current_state) {
    case States::Stopped: state_string = "Stopped";
    case States::Biking: state_string = "Biking";
    case States::Asleep: state_string = "Asleep";
  }
  Serial.printf("State: %s\tUp Button: %s\tDown Button: %s\tSpeed: %f\tCadence: %f\tGear: %i\n",
                state_string.data(),
                up_shift_button.get_button_state_string().data(), down_shift_button.get_button_state_string().data(),
                ground_estimator.get_speed(), pedal_cadence, shifter.current_gear());
}

bool safe_to_shift() {
  // TODO: find the value to enable shifting
  static constexpr auto SHIFT_PEDAL_CADENCE_CUTOFF{0.5f};  // [rpm]
  return pedal_cadence < SHIFT_PEDAL_CADENCE_CUTOFF;  // only shift while pedaling
}
