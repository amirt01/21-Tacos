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

void setup() {
  Serial.begin(115200);

  pinMode(REED_SWITCH_PIN, INPUT_PULLUP);
  pinMode(SHIFT_UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SHIFT_DOWN_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  switch(current_state) {
  }
}