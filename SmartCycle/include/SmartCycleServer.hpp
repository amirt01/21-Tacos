//
// Created by amirt on 3/15/2024.
//

#ifndef SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_
#define SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_

#include "WiFi.h"
#include "WebSocketsServer.h"
#include "ArduinoJson.hpp"

class SmartCycleServer {
  static constexpr char ssid[] = "SmartCycle";
  static constexpr char password[] = "bikesmart";

  WebSocketsServer web_socket = WebSocketsServer(80);

  hw_timer_s* broadcast_timer_config = timerBegin(0, divider, true);
  static constexpr auto broadcast_frequency = 5.;  // [Hz]
  static constexpr auto divider = APB_CLK_FREQ / 1e4;  // [Hz]
  static constexpr auto alarm_value = APB_CLK_FREQ / broadcast_frequency / divider;

  bool broadcast_flag{};

  ArduinoJson::JsonDocument j_doc;
  char j_str[256]{};

  static void web_socket_event_handler(byte num, WStype_t type, uint8_t*, size_t) {
    switch (type) {
      case WStype_DISCONNECTED:Serial.println("Client " + String(num) + " disconnected");
        break;
      case WStype_CONNECTED:Serial.println("Client " + String(num) + " connected");
        break;
      default:break;
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
    WiFi.softAP(ssid, password);

    web_socket.begin();
    web_socket.onEvent(web_socket_event_handler);

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
      web_socket.broadcastTXT(j_str, serializeJson(j_doc, j_str));
      broadcast_flag = false;
    }
  }

  // TODO: add flag variable so we only broadcast when there's new data to send
  template<typename T>
  void set(std::string_view key, T value) { j_doc[key] = value; }
};

#endif //SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_
