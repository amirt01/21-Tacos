#include <Arduino.h>
#include "esp_now.h"
#include "FastLED.h"

#include "Shifter.hpp"
#include "GroundEstimator.hpp"
#include "ButtonHandler.hpp"
#include "SmartCycleServer.hpp"

/** PINS **/
static constexpr uint8_t REED_SWITCH_PIN{27};
static constexpr uint8_t UP_SHIFT_BUTTON_PIN{14};
static constexpr uint8_t DOWN_SHIFT_BUTTON_PIN{13};
static constexpr uint8_t LED_PIN{12};
static constexpr uint8_t MOTOR_PIN{18};

/** STATES **/
enum class States {
  Biking,             // Coasting or Pedaling at constant speed
  Stopped,           // Waiting to bike again
  Asleep            // Low power mode
} current_state;

std::string_view state_str() {
  switch (current_state) {
    case States::Biking: return "Biking";
    case States::Stopped: return "Stopped";
    case States::Asleep: return "Asleep";
    default: return "wtf...";
  }
}

/** Server **/
auto& server = SmartCycleServer::get_instance();
void update_system_values();

/** CURRENT ESTIMATES **/
auto& ground_estimator = GroundEstimator<REED_SWITCH_PIN>::get_instance();
float cadence{};

/** SHIFTING **/
Shifter shifter{};
auto& up_shift_button = ButtonHandler<UP_SHIFT_BUTTON_PIN>::get_instance();
auto& down_shift_button = ButtonHandler<DOWN_SHIFT_BUTTON_PIN>::get_instance();

/** GEAR LEDs **/
CRGB gear_leds[Shifter::MAX_GEAR]{};
WS2813<LED_PIN, GRB> gear_led_controller{};
void update_gear_leds();

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

  /** Setup LEDs **/
  CFastLED::addLeds(&gear_led_controller, gear_leds, Shifter::MAX_GEAR);
  gear_led_controller.showLeds();

  /** Setup Ground Estimator **/
  ground_estimator.setup();

  /** Setup Buttons **/
  down_shift_button.setup();
  up_shift_button.setup();
}

void loop() {
  ground_estimator.loop();
  update_system_values();
  server.loop();

//  log();

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

      update_gear_leds();
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

      update_gear_leds();
      shifter.loop();
    }
  }
}

void update_system_values() {
  auto& msg = server.get_msg_ref();
  msg.speed = ground_estimator.get_speed();
  msg.cadence = cadence;
  msg.target_gear = shifter.get_target_gear();
  msg.current_gear = shifter.get_current_gear();
  msg.state = static_cast<SystemStatus_State>(current_state);
  msg.up_shift_button = static_cast<SystemStatus_ButtonState>(up_shift_button.state());
  msg.down_shift_button = static_cast<SystemStatus_ButtonState>(down_shift_button.state());
}

void update_gear_leds() {
  gear_led_controller.clearLedData();
  gear_led_controller[shifter.get_target_gear() - 1] = CRGB::Blue;
  gear_led_controller[shifter.get_current_gear() - 1] = CRGB::Green;
  gear_led_controller.showLeds();
}

void log() {
  Serial.printf("State: %s\tUp Button: %s\tDown Button: %s\tSpeed: %f\tCadence: %f\tCurrent Gear: %i\t"
                "Target Gear: %i\n",
                state_str().data(),
                up_shift_button.to_str().data(), down_shift_button.to_str().data(),
                ground_estimator.get_speed(), cadence, shifter.get_current_gear(),
                shifter.get_target_gear());
}

bool safe_to_shift() {
  // TODO: find the value to enable shifting
  static constexpr auto SHIFT_PEDAL_CADENCE_CUTOFF{0.5f};  // [rpm]
  return cadence < SHIFT_PEDAL_CADENCE_CUTOFF;  // only shift while pedaling
}
