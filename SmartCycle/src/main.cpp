#include <Arduino.h>

#include <bitset>

/* PINS */
static constexpr uint8_t REED_SWITCH_PIN{13};
static constexpr uint8_t SHIFT_UP_BUTTON_PIN{12};
static constexpr uint8_t SHIFT_DOWN_BUTTON_PIN{27};
static constexpr uint8_t MOTOR_PIN{18};

enum class States {
  Asleep,            // Low power mode
  Stopped,           // Waiting to bike again
  Shifting_Up,       // Shifting to higher gear
  Shifting_Down,     // Shifting to lower gear
  Biking             // Coasting or Pedaling at constant speed
} current_state;

enum class Transitions {
  Pedal_Start,        // Asleep/Stopped -> Biking
  Button_Wake,        // Asleep -> Stopped
  Push_Start,         // Asleep/Stopped -> Biking
  Falling_Asleep      // Biking/Stopped -> Asleep
} current_transition;

enum sensor_flag_ids {
  reed_switch,
  shift_up_button,
  shift_down_button,
  n_sensor_flags
};
std::bitset<n_sensor_flags> sensor_flags{};

template <uint8_t sensor_pin, sensor_flag_ids sensor_flag_id>
void setup_switch();

void setup() {
  Serial.begin(115200);
  
  setup_switch<REED_SWITCH_PIN, sensor_flag_ids::reed_switch>();
  setup_switch<SHIFT_UP_BUTTON_PIN, sensor_flag_ids::shift_up_button>();
  setup_switch<SHIFT_DOWN_BUTTON_PIN, sensor_flag_ids::shift_down_button>();
}

void loop() {
  switch (current_state) {
    case States::Sleep:
      break;
    case States::Stop:
      break;
    case States::Pedaling:
      break;
    default:
      break;
  }
}

template <uint8_t sensor_pin, sensor_flag_ids sensor_flag_id>
void setup_switch() {
  //TODO: test if INPUT_PULLUP is the best pin mode
  pinMode(sensor_pin, INPUT_PULLUP);

  // TODO: test to see if RISING is the best interrupt mode
  attachInterrupt(digitalPinToInterrupt(sensor_pin), [] { sensor_flags.set(sensor_flag_id); }, RISING);
}
