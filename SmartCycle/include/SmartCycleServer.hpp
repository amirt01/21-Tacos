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
  ArduinoJson::JsonObject j_object{j_doc.to<ArduinoJson::JsonObject>()};
  char j_str[128]{};

  static void web_socket_event(byte num, WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
      case WStype_DISCONNECTED:Serial.println("Client " + String(num) + " disconnected");
        break;
      case WStype_CONNECTED:Serial.println("Client " + String(num) + " connected");
        // optionally you can add code here what to do when connected
        break;
      default:break;
    }
  }

 public:
  void setup() {
    WiFi.softAP(ssid, password);

    web_socket.begin();
    web_socket.onEvent(web_socket_event);
  }

  void update() {
    web_socket.loop();

    web_socket.broadcastTXT(j_str, serializeJson(j_doc, j_str));
    j_doc.clear();
  }

  // TODO: add flag variable so we only broadcast when there's new data to send
  void set(std::string_view key, auto value) { j_object[key] = value; }
};

#endif //SMARTCYCLE_INCLUDE_SMARTCYCLESERVER_HPP_
