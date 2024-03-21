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

  ArduinoJson::JsonDocument j_doc{};
  char j_str[256]{};

  static void web_socket_event(byte num, WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
      case WStype_DISCONNECTED:Serial.println("Client " + String(num) + " disconnected");
        break;
      case WStype_CONNECTED:Serial.println("Client " + String(num) + " connected");
        break;
      default:break;
    }
  }

 public:
  void setup() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ssid, password);

    web_socket.begin();
    web_socket.onEvent(web_socket_event);
  }

  void update() {
    web_socket.loop();
    web_socket.broadcastTXT(j_str, serializeJson(j_doc, j_str));
  }

  // TODO: add flag variable so we only broadcast when there's new data to send
  template<typename T>
  void set(std::string_view key, T value) { j_doc[key] = value; }

  template<typename T>
  [[nodiscard]] T get(std::string_view key) const { return j_doc[key]; }

  void remove(std::string_view key) { j_doc.remove(key); }
};

#endif //SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_
