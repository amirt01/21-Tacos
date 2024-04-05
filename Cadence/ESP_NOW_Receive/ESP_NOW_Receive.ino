#include <esp_now.h>
#include <WiFi.h>

// Message
struct Message {
  float RPM_z;
} cadence;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&cadence, incomingData, sizeof(cadence));
  //Serial.print("Bytes received: ");
  //Serial.println(len);
  Serial.println(cadence.RPM_z);
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_AP_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB 
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

}