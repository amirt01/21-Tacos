//
// Created by amirt on 3/15/2024.
//

#ifndef SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_
#define SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_

#include <pb_encode.h>
#include "SmartCycle.pb.h"

#include "WiFi.h"
#include "WebSocketsServer.h"

class SmartCycleServer {
  // Access point variables
  static constexpr char ssid[] = "SmartCycle";
  static constexpr char password[] = "bikesmart";

  // Web socket server initialization
  WebSocketsServer web_socket = WebSocketsServer(80);

  static void web_socket_event_handler(byte num, WStype_t type, uint8_t*, size_t) {
    switch (type) {
      case WStype_DISCONNECTED:Serial.println("Client " + String(num) + " disconnected");
        break;
      case WStype_CONNECTED:Serial.println("Client " + String(num) + " connected");
        break;
      default:break;
    }
  }

  // Broadcast timer initialization
  static constexpr auto publish_rate = 1e5;  // [us]
  bool broadcast_flag{};
  esp_timer_handle_t broadcast_timer;
  esp_timer_create_args_t timer_args{
      [](void* flag_ptr) { *static_cast<bool*>(flag_ptr) = true; },
      &broadcast_flag,
      ESP_TIMER_TASK,
      "broadcast timer",
      true
  };

  // Protobuffer initialization
  ServerStatus status_msg = ServerStatus_init_default;
  pb_ostream_t stream{
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
      if (!pb_encode_nullterminated(&stream, ServerStatus_fields, &status_msg)) {
        Serial.printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
      }

      broadcast_flag = false;
    }
  }

  // TODO: add flag variable so we only broadcast when there's new data to send
  ServerStatus& get_msg_ref() { return status_msg; }
};

#endif //SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_
