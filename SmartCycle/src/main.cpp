#include <Arduino.h>

#include "GroundEstimator.hpp"

/* PINS */
static constexpr uint8_t REED_SWITCH_PIN{13};
static constexpr uint8_t SHIFT_UP_BUTTON_PIN{12};
static constexpr uint8_t SHIFT_DOWN_BUTTON_PIN{27};
static constexpr uint8_t MOTOR_PIN{18};

/* FLAG VARIABLES */
// TODO: create button objects for testing
bool shift_up_button_flag;
bool shift_down_button_flag;

/* STATES */
enum class States {
  Asleep,            // Low power mode
  Stopped,           // Waiting to bike again
  Shifting_Up,       // Shifting to higher gear
  Shifting_Down,     // Shifting to lower gear
  Biking             // Coasting or Pedaling at constant speed
} current_state = States::Asleep;

/* CURRENT ESTIMATES */
auto& ground_estimator = GroundEstimator<REED_SWITCH_PIN>::get_ground_estimator();    // [meters per second]
float pedal_cadence{};   // [rpm]
float acceleration{};    // [meters per second per second]

/* RUN FUNCTIONS */
void update_shifter(int8_t direction) {  // -1 shifting down; 0 holding; +1 shifting up
  // TODO: implement this
}

void update_anticipations() {
  // TODO: implement this
  // anticipate using the camera (i.e. slowing down via stop sign, traffic light, object on road, etc)
}

bool safe_to_shift();

void setup() {
  Serial.begin(115200);

  auto setup_switch = [] (const uint8_t pin, void (*ISR)()) {
    //TODO: test if INPUT_PULLUP is the best pin mode
    pinMode(pin, INPUT_PULLUP);
    // TODO: test to see if RISING is the best interrupt mode
    attachInterrupt(digitalPinToInterrupt(pin), ISR, RISING);
  };

  setup_switch(REED_SWITCH_PIN, [] { ground_estimator.set_reed_switch_flag(); });
  setup_switch(SHIFT_UP_BUTTON_PIN, [] { shift_up_button_flag = true; });
  setup_switch(SHIFT_DOWN_BUTTON_PIN, [] { shift_down_button_flag = true; });
}

void loop() {
  switch (current_state) {
    case States::Asleep: {
      if (shift_up_button_flag) {
        shift_up_button_flag = false;
        current_state = States::Stopped;
      } else if (shift_down_button_flag) {
        shift_down_button_flag = false;
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

      update_shifter(0);
      break;
    }
    case States::Shifting_Up: {
      update_anticipations();
      if (safe_to_shift()) {
        update_shifter(1);
      }
      if (true) {  // TODO: add check for done shifting
        current_state = States::Biking;
      }
      break;
    }
    case States::Shifting_Down: {
      update_anticipations();

      if (safe_to_shift()) {
        update_shifter(-1);
      }
      if (true) {  // TODO: add check for done shifting
        current_state = States::Biking;
      }
      break;
    }
    case States::Biking: {
      // TODO: find the stop speed cutoff value
      static constexpr auto STOP_SPEED_CUTOFF{0.1};  // [meters per second]
      if (ground_estimator.get_speed() < STOP_SPEED_CUTOFF) {
        current_state = States::Stopped;
        break;
      }

      update_anticipations();
      update_shifter(0);

      // TODO: find the value
      static constexpr auto SHIFT_ACCELERATION_CUTOFF{0.5f};   // [meters per second per second]
      if (shift_up_button_flag) {
        shift_up_button_flag = false;
        current_state = States::Shifting_Up;
      } else if (shift_down_button_flag) {
        shift_down_button_flag = false;
        current_state = States::Shifting_Down;
      } else if (acceleration > SHIFT_ACCELERATION_CUTOFF) {
        current_state = States::Shifting_Up;
      } else if (acceleration < -SHIFT_ACCELERATION_CUTOFF) {
        current_state = States::Shifting_Down;
      }
      break;
    }
  }
}

bool safe_to_shift() {
  // TODO: find the value to enable shifting
  static constexpr auto SHIFT_PEDAL_CADENCE_CUTOFF{0.5f};  // [rpm]
  return pedal_cadence < SHIFT_PEDAL_CADENCE_CUTOFF;  // only shift while pedaling
}
