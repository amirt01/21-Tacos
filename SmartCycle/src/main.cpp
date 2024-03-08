#include <Arduino.h>

#include <bitset>

static constexpr auto REED_SWITCH_PIN{13};
static constexpr auto SHIFT_UP_BUTTON_PIN{12};
static constexpr auto SHIFT_DOWN_BUTTON_PIN{27};

enum class States {
    Sleeping,
    Waiting,
    Coasting,
    Shifting
} current_state;

void reed_switch_isr();
void shift_up_button_isr();
void shift_down_button_isr();

enum sensor_flag_ids {
  reed_switch,
  shift_up_button,
  shift_down_button,
  n_sensor_flags
};
std::bitset<n_sensor_flags> sensor_flags{};

void setup() {
  Serial.begin(115200);

  pinMode(REED_SWITCH_PIN, INPUT_PULLUP);
  pinMode(SHIFT_UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SHIFT_DOWN_BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(REED_SWITCH_PIN), reed_switch_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(REED_SWITCH_PIN), shift_up_button_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(REED_SWITCH_PIN), shift_down_button_isr, FALLING);
}

void loop() {
  switch(current_state) {
  }
}

void reed_switch_isr() {
  sensor_flags.set(sensor_flag_ids::reed_switch);
}

void shift_up_button_isr() {
  sensor_flags.set(sensor_flag_ids::shift_up_button);
  
}

void shift_down_button_isr() {
  sensor_flags.set(sensor_flag_ids::shift_down_button);
}
