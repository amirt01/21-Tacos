//
// Created by amirt on 3/15/2024.
//

#ifndef SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_
#define SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_

#include <pb_encode.h>
#include <pb_decode.h>
#include "SmartCycle.pb.h"

#include "WiFi.h"
#include "WebSocketsServer.h"

class SmartCycleServer {
  // Access point variables
  static constexpr char ssid[] = "SmartCycle";
  static constexpr char password[] = "bikesmart";

  // Web socket server initialization
  WebSocketsServer web_socket = WebSocketsServer(80);

  static void web_socket_event_handler(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
      case WStype_DISCONNECTED: Serial.printf("Client %i disconnected\n", num);
        break;
      case WStype_CONNECTED: {
        Serial.printf("Client %i connected\n", num);

        // Send the initial tuning values
        auto& instance = SmartCycleServer::get_instance();
        instance.send_to_socket(instance.tuning_msg);

        break;
      }
      case WStype_BIN: {
        // Wrap the payload into a nanopb input stream
        auto tuning_stream = pb_istream_from_buffer(payload, length);

        // Decode the stream without overwriting the previously set values
        auto& instance = SmartCycleServer::get_instance();
        if (!pb_decode_noinit(&tuning_stream, Tuning_fields, &instance.tuning_msg)) {
          Serial.printf("Encoding failed: %s\n", PB_GET_ERROR(&tuning_stream));
        }

        break;
      }
      default:break;
    }
  }

  // Broadcast timer initialization
  static constexpr auto publish_rate = 2e5;  // [us]
  bool broadcast_flag{};
  esp_timer_handle_t broadcast_timer{};
  esp_timer_create_args_t timer_args{
      [](void* flag_ptr) { *static_cast<bool*>(flag_ptr) = true; },
      &broadcast_flag,
      ESP_TIMER_TASK,
      "broadcast timer",
      true
  };

  // Define IO messages
  Message telemetry_msg = {Message_telemetry_tag, {.telemetry=Telemetry_init_default}};
  Message tuning_msg = {Message_tuning_tag, {.tuning=Tuning_init_default}};

  void send_to_socket(const Message& msg) {
    static pb_ostream_t socket_stream{
        [](pb_ostream_t* stream, const pb_byte_t* buf, size_t count) {
          return static_cast<WebSocketsServer*>(stream->state)->broadcastBIN(buf, count);
        },
        &web_socket,
        SIZE_MAX,
        0
    };

    if (!pb_encode_nullterminated(&socket_stream, Message_fields, &msg)) {
      Serial.printf("Encoding failed: %s\n", PB_GET_ERROR(&socket_stream));
    }
  }

  SmartCycleServer() = default;

 public:
  SmartCycleServer(SmartCycleServer const&) = delete;
  void operator=(SmartCycleServer const&) = delete;

  static SmartCycleServer& get_instance() {
    static SmartCycleServer scs{};
    return scs;
  }

  void setup() {
    // Set up Access Point
    WiFi.softAP(ssid, password);

    // Start web socket handling
    web_socket.begin();
    web_socket.onEvent(web_socket_event_handler);

    // Start broadcast timer
    esp_timer_create(&timer_args, &broadcast_timer);
    esp_timer_start_periodic(broadcast_timer, publish_rate);
  }

  void loop() {
    web_socket.loop();

    if (web_socket.connectedClients() && broadcast_flag) {
      send_to_socket(telemetry_msg);
      broadcast_flag = false;
    }
  }

  // TODO: add flag variable so we only broadcast when there's new data to send
  void set_telemetry_msg(const Telemetry& telemetry) { telemetry_msg.packet.telemetry = telemetry; }
  void set_tuning_msg(const Tuning& tuning) { tuning_msg.packet.tuning = tuning; }
};

#endif //SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_
