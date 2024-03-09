#include <Arduino.h>

/* PINS */
static constexpr uint8_t REED_SWITCH_PIN{13};
static constexpr uint8_t SHIFT_UP_BUTTON_PIN{12};
static constexpr uint8_t SHIFT_DOWN_BUTTON_PIN{27};
static constexpr uint8_t MOTOR_PIN{18};

/* BIKE CONSTANTS*/
static constexpr float WHEEL_DIAMETER{1};  // [m] // TODO: check this

enum class States {
  Asleep,            // Low power mode
  Stopped,           // Waiting to bike again
  Shifting_Up,       // Shifting to higher gear
  Shifting_Down,     // Shifting to lower gear
  Biking             // Coasting or Pedaling at constant speed
} current_state;

bool reed_switch_flag;
bool shift_up_button_flag;
bool shift_down_button_flag;

/* CURRENT ESTIMATES */
float ground_speed{};    // [meters per second]
uint8_t current_gear{1};  // [1-6]
float acceleration{};

/* SETUP FUNCTIONS */
template <uint8_t sensor_pin, bool& flag>
void setup_switch();

/* RUN FUNCTIONS */
void update_shifter(int8_t direction) {  // -1 shifting down; 0 holding; +1 shifting up
  // TODO: add PID control
  float shift_amount = direction * 0.01;
  digitalWrite(MOTOR_PIN, shift_amount);
};

void update_anticipations() {
  // TODO: implement this
  // anticipate using the camera (i.e. slowing down via stop sign, traffic light, object on road, etc)
}

void update_ground_speed(const unsigned long current_time) {
  static unsigned long last_reed_time{};

  if (reed_switch_flag) {
    ground_speed = WHEEL_DIAMETER * M_PI / (current_time - last_reed_time);

    last_reed_time = current_time;
    reed_switch_flag = false;
  }
}

void setup() {
  Serial.begin(115200);
  
  setup_switch<REED_SWITCH_PIN, reed_switch_flag>();
  setup_switch<SHIFT_UP_BUTTON_PIN, shift_up_button_flag>();
  setup_switch<SHIFT_DOWN_BUTTON_PIN, shift_down_button_flag>();
}

void loop() {
  const auto current_time = millis();

  /* SENSOR UPDATES */
  update_ground_speed(current_time);

  switch (current_state) {
    case States::Asleep:
      if (shift_up_button_flag) {
        shift_up_button_flag = false;
        current_state = States::Stopped;
      } else if (shift_down_button_flag) {
        shift_up_button_flag = false;
        current_state = States::Stopped;
      } else if (ground_speed > 0.1) {
        current_state = States::Biking;
      }
      break;
    case States::Stopped:
      if (ground_speed > 0.1) {
        current_state = States::Biking;
      }

      update_shifter(0);
      break;
    case States::Shifting_Up:
      update_anticipations();
      update_shifter(1);
      if (true) {  // TODO: add check for done shifting
        current_state = States::Biking;
      }
      break;
    case States::Shifting_Down:
      update_anticipations();
      update_shifter(-1);
      if (true) {  // TODO: add check for done shifting
        current_state = States::Biking;
      }
      break;
    case States::Biking:
      update_anticipations();
      update_shifter(0);
      break;
  }
}

template <uint8_t sensor_pin, bool& flag>
void setup_switch() {
  //TODO: test if INPUT_PULLUP is the best pin mode
  pinMode(sensor_pin, INPUT_PULLUP);
  
  // TODO: test to see if RISING is the best interrupt mode
  attachInterrupt(digitalPinToInterrupt(sensor_pin), [] { flag = true; }, RISING);
}
