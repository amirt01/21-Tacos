//
// Created by amirt on 3/14/2024.
//
#include <Arduino.h>

#include "WiFi.h"
#include "WebSocketsServer.h"
#include "ArduinoJson.hpp"

const char* ssid = "SmartCycle";
const char* password = "bikesmart";

WebSocketsServer web_socket = WebSocketsServer(80);
void web_socket_event(byte num, WStype_t type, uint8_t* payload, size_t length);

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  web_socket.begin();
  web_socket.onEvent(web_socket_event);
}

void loop() {
  web_socket.loop();

  static unsigned long previous_loop{};
  if (unsigned long now = millis(); now > previous_loop) {
    web_socket.broadcastTXT("");

    String jsonString = "";
    ArduinoJson::JsonDocument doc;
    ArduinoJson::JsonObject object = doc.to<ArduinoJson::JsonObject>();
    object["speed"] = random(0, 45);
    object["cadence"] = random(0, 120);
    object["gear"] = random(1, 6);
    serializeJson(doc, jsonString);
    Serial.println(jsonString);
    web_socket.broadcastTXT(jsonString);

    previous_loop = now + 500;
  }
}

void web_socket_event(byte num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:Serial.println("Client " + String(num) + " disconnected");
      break;
    case WStype_CONNECTED:Serial.println("Client " + String(num) + " connected");
      // optionally you can add code here what to do when connected
      break;
    default:break;
  }
}
