//
// Created by amirt on 3/14/2024.
//
#include <Arduino.h>

#include "WiFi.h"
#include "ESPAsyncWebServer.h"

const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

AsyncWebServer server(80);

void setup(){
  Serial.begin(115200);
  Serial.println();

  char acc_data[30];

  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/cadence", HTTP_GET, [i=0](AsyncWebServerRequest *request) mutable {
    char buffer[10];
    request->send_P(200, "text/plain", itoa(i++, buffer, 10));
  });

  server.begin();
}

void loop(){

}
