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

  // Broadcast timer calculations and initialization
  static constexpr auto broadcast_frequency = 5.;  // [Hz]
  static constexpr auto divider = APB_CLK_FREQ / 1e4;  // [Hz]
  static constexpr auto alarm_value = APB_CLK_FREQ / broadcast_frequency / divider;
  hw_timer_s* broadcast_timer_config = timerBegin(0, divider, true);
  bool broadcast_flag{};

  // Broadcast packet initialization
  ServerStatus status_msg{};
  uint8_t buffer[126]{};
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

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
    timerAttachInterrupt(
        broadcast_timer_config,
        [] { SmartCycleServer::get_instance().broadcast_flag = true; },
        true);
    timerAlarmWrite(broadcast_timer_config, alarm_value, true);
    timerAlarmEnable(broadcast_timer_config);
  }

  void loop() {
    web_socket.loop();

    if (broadcast_flag) {
      if (!pb_encode(&stream, ServerStatus_fields, &status_msg)) {
        Serial.printf("Encoding failed: %s\n", PB_GET_ERROR((&stream)));
        assert(0);
      }

      web_socket.broadcastBIN((uint8_t*)stream.state, stream.bytes_written);
      broadcast_flag = false;
    }
  }

  // TODO: add flag variable so we only broadcast when there's new data to send
  ServerStatus& get_msg_ref() { return status_msg; }
};

#endif //SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_
