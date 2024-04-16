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

constexpr auto SPEED_CUTOFF{0.1};  // [meters per second]

/** Server **/
auto& server = SmartCycleServer::get_instance();
void update_telemetry_values();
void update_tuning_values();

/** CURRENT ESTIMATES **/
auto& ground_estimator = GroundEstimator<REED_SWITCH_PIN>::get_instance();
float cadence{100};

/** SHIFTING **/
Shifter shifter{};
auto& up_shift_button = ButtonHandler<UP_SHIFT_BUTTON_PIN>::get_instance();
auto& down_shift_button = ButtonHandler<DOWN_SHIFT_BUTTON_PIN>::get_instance();
[[nodiscard]] uint8_t calculate_optimal_gear() noexcept;

/** GEAR LEDs **/
CRGB gear_leds[Shifter::MAX_GEAR]{};
WS2813<LED_PIN, GRB> gear_led_controller{};
void update_gear_leds();

/** RUN FUNCTIONS **/
[[maybe_unused]] void log();
[[maybe_unused]] void demo_loop();

void setup() {
  Serial.begin(115200);

  /** Setup Wireless Communications **/
  WiFiClass::mode(WIFI_AP_STA);

  server.setup();
  update_tuning_values();

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
  update_telemetry_values();
  server.loop();
  update_gear_leds();
  shifter.loop();

  demo_loop();
  log();

  switch (current_state) {
    case States::Asleep: {
      if (up_shift_button || down_shift_button) {
        current_state = States::Stopped;
      } else if (ground_estimator.get_speed() > SPEED_CUTOFF) {
        current_state = States::Biking;
      }
      break;
    }
    case States::Stopped: {
      if (ground_estimator.get_speed() > SPEED_CUTOFF) {
        current_state = States::Biking;
      }

      update_gear_leds();
      shifter.loop();
      break;
    }
    case States::Biking: {
      // TODO: find the stop speed cutoff value
      if (ground_estimator.get_speed() < SPEED_CUTOFF) {
        shifter.reset();
        current_state = States::Stopped;
        break;
      }

      switch (shifter.shift_mode) {
        case Shifter::shift_mode::AUTOMATIC:
          if (!up_shift_button == !down_shift_button) {
            shifter.set_target_gear(calculate_optimal_gear());
            break;
          } else {
            shifter.shift_mode = Shifter::shift_mode::MANUAL;
            [[fallthrough]];
          }
        case Shifter::shift_mode::MANUAL:
          if (up_shift_button && down_shift_button) {
            shifter.shift_mode = Shifter::shift_mode::AUTOMATIC;
          } else if (up_shift_button) {
            shifter.shift(Shifter::shift_direction::UP);
          } else if (down_shift_button) {
            shifter.shift(Shifter::shift_direction::DOWN);
          }
      }
    }
  }
}

void update_telemetry_values() {
  server.set_telemetry_msg(
      {
          ground_estimator.get_speed(),
          cadence,
          shifter.get_target_gear(),
          shifter.get_current_gear(),
          static_cast<Telemetry_State>(current_state),
          static_cast<Telemetry_ButtonState>(up_shift_button.state()),
          static_cast<Telemetry_ButtonState>(down_shift_button.state())
      }
  );
}

void update_tuning_values() {
  auto& nominal_gear_encoder_values = shifter.get_nominal_gear_encoder_values_ref();
  server.set_tuning_msg(
      {
          nominal_gear_encoder_values[0],
          nominal_gear_encoder_values[1],
          nominal_gear_encoder_values[2],
          nominal_gear_encoder_values[3],
          nominal_gear_encoder_values[4],
          nominal_gear_encoder_values[5],
          120,
          90
      }
  );
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

[[nodiscard]] uint8_t calculate_optimal_gear() noexcept {
  // Calculate the speed ratio between the crank [rpm] and the ground [kph]
  auto speed_ratio = [](const uint16_t cog_diameter) {
    static constexpr uint16_t chainring_diameter = 36; // [mm]
    static constexpr uint16_t wheel_diameter = 622;    // [mm]
    static constexpr uint16_t tire_size = 23;          // [mm]

    return static_cast<float>(M_PI) * (wheel_diameter + 2 * tire_size) / 1000 * chainring_diameter / cog_diameter;
  };

  // Calculate the nominal speed for a cog at the current pedal cadence
  auto nominal_speed = [speed_ratio](const uint16_t cog_diameter) {
    return speed_ratio(cog_diameter) * cadence / 60 * 3.6;
  };

  // Calculate the difference in speed between the current speed and the nominal speed for a cog
  auto speed_dif = [nominal_speed](const uint16_t cog_diameter) {
    return std::abs(ground_estimator.get_speed() - nominal_speed(cog_diameter));
  };

  auto speed_dif_cmp = [speed_dif](int cog1, int cog2) {
    return speed_dif(cog1) < speed_dif(cog2);
  };

  static constexpr std::array<uint16_t, 6> cassette = {28, 24, 21, 18, 16, 14}; // [mm]
  auto optimal_gear_itr = std::min_element(cassette.cbegin(), cassette.cend(), speed_dif_cmp);
  return std::distance(cassette.begin(), optimal_gear_itr) + 1;
}

void demo_loop() {
  static float encoder_value = 50;

  shifter.set_encoder_value(encoder_value);
  shifter.loop();
  encoder_value += ceil(0.025 * shifter.get_motor_signal()) - random(0, 4) + random(0, 4);
}
