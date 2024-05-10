/* 
* Reed Switch Simulator
* 
* Chris Simotas 4/27/2024
*
* Send high signals to pin to simulate Reed switch being hit. To be uploaded to seperate board
*
*/

#include "Arduino.h"
#include <esp_timer.h>

// Pulse Timer
esp_timer_handle_t timer_pulser;
int sampleRate = 500000;

void pulser(void *arg){
    
    digitalWrite(2, LOW);
    delay(10);
    digitalWrite(2, HIGH);
    
}

// Create Arguments for Timer
esp_timer_create_args_t timer_args = {
  .callback = pulser,
  .name = "pulser"
};


void setup() {
    pinMode(2, OUTPUT);
    
    // Create Pulser Timer
    esp_timer_create(&timer_args, &timer_pulser);
    esp_timer_start_periodic(timer_pulser, sampleRate);
}

void loop() {


    



}