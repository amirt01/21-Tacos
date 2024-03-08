#include <Arduino.h>

constexpr auto REED_SWITCH_PIN{13};
constexpr auto SHIFT_UP_BUTTON_PIN{12};
constexpr auto SHIFT_DOWN_BUTTON_PIN{27};

enum class States {
    Sleeping,
    Waiting,
    Coasting,
    Shifting
} current_state;

void reed_switch_isr();
void shift_up_button_isr();
void shift_down_button_isr();

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