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

  static void hexdump(const void* mem, uint32_t len, uint8_t cols = 16) {
    const uint8_t* src = (const uint8_t*) mem;
    Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t) src, len, len);
    for (uint32_t i = 0; i < len; i++) {
      if (i % cols == 0) {
        Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t) src, i);
      }
      Serial.printf("%02X ", *src);
      src++;
    }
    Serial.printf("\n");
  }

  static void web_socket_event_handler(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
      case WStype_DISCONNECTED: {
        Serial.printf("Client %i disconnected\n", num);
        break;
      }
      case WStype_CONNECTED: {
        Serial.printf("Client %i connected\n", num);
        break;
      }
      case WStype_BIN: {
        Serial.printf("[%u] get binary length: %u\n", num, length);
        hexdump(payload, length);
      }
      default:
        break;
    }
  }

  // Broadcast timer initialization
  static constexpr auto publish_rate = 1e5;  // [us]
  bool broadcast_flag{};
  esp_timer_handle_t broadcast_timer{};
  esp_timer_create_args_t timer_args{
      [](void* flag_ptr) { *static_cast<bool*>(flag_ptr) = true; },
      &broadcast_flag,
      ESP_TIMER_TASK,
      "broadcast timer",
      true
  };

  // Protobuffer initialization
  SystemStatus status_msg = SystemStatus_init_default;
  pb_ostream_t status_stream{
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
      if (!pb_encode_nullterminated(&status_stream, SystemStatus_fields, &status_msg)) {
        Serial.printf("Encoding failed: %s\n", PB_GET_ERROR(&status_stream));
      }

      broadcast_flag = false;
    }
  }

  // TODO: add flag variable so we only broadcast when there's new data to send
  SystemStatus& get_msg_ref() { return status_msg; }
};

#endif //SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_
