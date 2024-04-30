/**********************************************************
* Wireless Cadende Integrator
* v2.0 with timer
*
* Chris Simotas
* ME 235 Final Project
*
* Smart Cycle
* Timer Docs: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_timer.html#freertos-timers
*
**********************************************************/

#include <M5Unified.h>
#include <WiFi.h>
#include <esp_now.h>
#include <math.h>
#include <esp_timer.h>

// Initialize Calibration Variables
static unsigned long startTime = millis();
unsigned long currentTime;
float sumGyroBias = 0.0;
float estGyroBias = 0.0;
int biasCount = 0;

// Initialize Moving Average Variables
//const int numReadings = 3; 
//float mv_avg[numReadings] = {0.0};
//int i = 0;               
float total = 0.0; 
int smoothCount = 0;
int sampleRate = 100000;

// Receiver MAC Address
uint8_t broadcastAddress[] = {0xD8, 0xBC, 0x38, 0xFC, 0xCC, 0x4C}; // Modify accordingly
esp_now_peer_info_t peerInfo;

// Message Structure 
typedef struct Message {
  float RPM_z;
} Message;

Message cadence;

// Cadence Smooth Data Callback
void smooth_data(void *arg){
  
  cadence.RPM_z = total / float(smoothCount);
  //Serial.println(cadence.RPM_z);

  smoothCount = 0;
  total = 0.0;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&cadence, sizeof(cadence));
  //Serial.println(result == ESP_OK ? "\nMessage Send Success" : "\nMessenge Send Fail");

}

// Moving Average Timer
esp_timer_handle_t timer_smooth_data;

// Create Arguments for Moving Average Timer
esp_timer_create_args_t timer_args = {
  .callback = smooth_data,
  .name = "smooth_data"
};


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

  /*
  M5.Lcd.setCursor(0, 40);
  M5.Lcd.clear(); 
  M5.Lcd.printf("Cadence: %0.2f\r\n", cadence.RPM_z);
  M5.Lcd.printf(status == ESP_NOW_SEND_SUCCESS ? "\nDelivery Success" : "\nDelivery Fail");
  */
}


void setup(void) {
  
  // Configure M5 and Serial
  auto cfg = M5.config();
  M5.begin(cfg);
  Serial.begin(115200);

  // Set device as a Wi-Fi Station and ESP-NOW
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to get the status of Trasmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register and Add Peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Perform calibration ////////////////////////////////
  delay(3000); 

  while (millis() - startTime < 4000) {
    auto imu_update = M5.Imu.update();
    
    if (imu_update) {
      auto data = M5.Imu.getImuData();
      sumGyroBias = sumGyroBias + data.gyro.z;
      biasCount += 1;

      Serial.printf("Calibrating...\n");
    }
  }
  
  estGyroBias = sumGyroBias / float(biasCount);
  Serial.printf("Calibration Complete!\n");
  M5.Lcd.setCursor(0, 40);
  M5.Lcd.clear();
  M5.Lcd.printf("CALIBRATION COMPLETE!");
  delay(3000);
  M5.Lcd.clear();
  //////////////////////////////////////////////////

  // Create Smooth Data Timer
  esp_timer_create(&timer_args, &timer_smooth_data);
  esp_timer_start_periodic(timer_smooth_data, sampleRate);
}

void loop() {
  
  auto imu_update = M5.Imu.update();

  if (imu_update) {
    auto data = M5.Imu.getImuData();

    // Calculate RPM_z
    float RPM_z = ((data.gyro.z - estGyroBias) * 0.1667); // 1/360 * 60 = 0.1667

    total += RPM_z;
    smoothCount += 1;

  /*
    // Populate the Cadence Structure
    total -= mv_avg[i];
    mv_avg[i] = RPM_z;
    total += RPM_z;
    i = (i+1) % numReadings;
    cadence.RPM_z = total / (float)numReadings;


    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&cadence, sizeof(cadence));

    //M5.Lcd.printf("Cadence: %0.2f\r\n", RPM_z);
    //M5.Lcd.printf(result == ESP_OK ? "\nMessage Send Success" : "\nMessenge Send Fail");
    //Serial.println(result == ESP_OK ? "\nMessage Send Success" : "\nMessenge Send Fail");
  */
  }

}

