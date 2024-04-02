#include <M5Unified.h>
#include <WiFi.h>
#include <esp_now.h>
#include <math.h>

// Initialize Variables
static unsigned long startTime = millis();
unsigned long currentTime;
//float sumGyroBias[3] = {0.0};
//float estGyroBias[3] = {0.0};
float sumGyroBias = 0.0;
float estGyroBias = 0.0;
bool flag = false;
int counter;

// Initialize Moving Average Variables
const int numReadings = 3; 
float mv_avg[numReadings] = {0.0};
int i = 0;               
float total = 0.0; 

// Receiver MAC Address
uint8_t broadcastAddress[] = {0xD8, 0xBC, 0x38, 0xFC, 0xF3, 0xD4}; // Modify accordingly
esp_now_peer_info_t peerInfo;

// Message Structure 
typedef struct Message {
  float RPM_z;
} Message;

Message cadence;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  M5.Lcd.setCursor(0, 40);
  M5.Lcd.clear(); 
  M5.Lcd.printf("Cadence: %0.2f\r\n", cadence.RPM_z);
  M5.Lcd.printf(status == ESP_NOW_SEND_SUCCESS ? "\nDelivery Success" : "\nDelivery Fail");
}

void setup(void) {
  
  // Configure M5
  auto cfg = M5.config();
  M5.begin(cfg);

  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to get the status of Trasmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop(void) {
  auto imu_update = M5.Imu.update();

  if (imu_update) {
    auto data = M5.Imu.getImuData();

    if (!flag) {

      if (millis() - startTime < 3000) {
        //sumGyroBias[0] = sumGyroBias[0] + data.gyro.x;
        //sumGyroBias[1] = sumGyroBias[1] + data.gyro.y;
        sumGyroBias = sumGyroBias + data.gyro.z;
        counter += 1;

        Serial.printf("Calibrating...\n");
      } else {
        //estGyroBias[0] = sumGyroBias[0] / counter;
        //estGyroBias[1] = sumGyroBias[1] / counter;
        estGyroBias = sumGyroBias / counter;
        flag = true;

        Serial.printf("Calibration Complete!\n");
        M5.Lcd.setCursor(0, 40);
        M5.Lcd.clear();
        M5.Lcd.printf("CALIBRATION COMPLETE!");
        delay(2000);
      }
    } else {

      //M5.Lcd.setCursor(0, 40);
      //M5.Lcd.clear(); 

      // Calculate normalized gyro values
      //float RPM_x = ((data.gyro.x - estGyroBias[0]) / 360);
      //float RPM_y = ((data.gyro.y - estGyroBias[1]) / 360);
      float RPM_z = ((data.gyro.z - estGyroBias) / 360 * 60);

      // Populate the cadence struct
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
    }
  }

  // Delay to control the rate of sending data
  delay(5);
}
