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

        auto& instance = SmartCycleServer::get_instance();

        // Send the initial tuning values
        Message initial_tuning_msg{};
        initial_tuning_msg.which_packet = Message_tuning_tag;
        initial_tuning_msg.packet.tuning = instance.tuning_msg;
        if (!pb_encode_nullterminated(&instance.socket_stream, Message_fields, &initial_tuning_msg)) {
          Serial.printf("Encoding failed: %s\n", PB_GET_ERROR(&instance.socket_stream));
        }

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
  static constexpr auto publish_rate = 3e5;  // [us]
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
  Message initial_tuning_msg = {Message_telemetry_tag, Telemetry_init_default};
  Tuning tuning_msg = Tuning_init_default;

  // Protobuffer initialization
  pb_ostream_t socket_stream{
      [](pb_ostream_t* stream, const pb_byte_t* buf, size_t count) {
        return static_cast<WebSocketsServer*>(stream->state)->broadcastBIN(buf, count);
      },
      &web_socket,
      SIZE_MAX,
      0
  };

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
    if (!web_socket.connectedClients()) {
      return;
    }

    if (broadcast_flag) {
      if (!pb_encode_nullterminated(&socket_stream, Message_fields, &initial_tuning_msg)) {
        Serial.printf("Encoding failed: %s\n", PB_GET_ERROR(&socket_stream));
      }

      broadcast_flag = false;
    }
  }

  // TODO: add flag variable so we only broadcast when there's new data to send
  Telemetry& get_telemetry_msg_ref() { return initial_tuning_msg.packet.telemetry; }
  Tuning& get_tuning_msg_ref() { return initial_tuning_msg.packet.tuning; }
};

#endif //SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_
