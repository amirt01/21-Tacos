
// #include "Encoder.hpp"

#include <ESP32Encoder.h>

#include <Arduino.h>

static constexpr uint8_t ENC_A_PIN{5};
static constexpr uint8_t ENC_B_PIN{18};

static constexpr uint8_t R_PWM_PIN{19};
static constexpr uint8_t L_PWM_PIN{21};

static constexpr uint8_t R_PWM_CHAN{0};
static constexpr uint8_t L_PWM_CHAN{1};

ESP32Encoder encoder;

// Encoder<ENC_A_PIN, ENC_B_PIN> encoder{};

void setup(){
    Serial.begin(115200);
    pinMode(R_PWM_PIN, OUTPUT);
    pinMode(L_PWM_PIN, OUTPUT);

    ledcSetup(R_PWM_CHAN, 1000, 10); 
    ledcAttachPin(R_PWM_PIN, R_PWM_CHAN);

    ledcSetup(L_PWM_CHAN, 1000, 10); 
    ledcAttachPin(L_PWM_PIN, L_PWM_CHAN);

    encoder.attachHalfQuad(5, 18);
    encoder.setCount(0);
}


long prevPrintTimeMs = millis();

long prevDirTimeMs = millis();

int direction = 0;

int setpoint = 20000;

void loop(){
    // encoder.loop();

    long curTimeMs = millis();

    if(curTimeMs - prevDirTimeMs < 10000){
        setpoint = 3000;
    } else if (curTimeMs - prevDirTimeMs < 20000){
        setpoint = 0;
    } else if (curTimeMs - prevDirTimeMs < 30000){
        setpoint = -3000;
    } else {
        prevDirTimeMs = curTimeMs;
    }

    int cur_enc_val = encoder.getCount();

    int diff = cur_enc_val - setpoint;

    int motor_speed = 0;

    int buffer = 1;

    motor_speed = (abs((int)diff) * 2 + 128);

    if(motor_speed > 1023){
        motor_speed = 1023;
    }

    if(diff < 0){
        motor_speed *= -1;
    }

    if(abs(diff) < buffer){
        motor_speed = 0;
    }

    if(motor_speed > 0){
        ledcWrite(R_PWM_CHAN, 0);
        ledcWrite(L_PWM_CHAN, abs(motor_speed));
    } else if (motor_speed < 0){
        ledcWrite(R_PWM_CHAN, abs(motor_speed));
        ledcWrite(L_PWM_CHAN, 0);
    } else {
        ledcWrite(R_PWM_CHAN, 0);
        ledcWrite(L_PWM_CHAN, 0);
    }

    if(curTimeMs - prevPrintTimeMs > 250){
        Serial.printf("Encoder Value: %i | Setpoint: %i | Diff: %i | Motor Speed: %i \n", cur_enc_val, setpoint, diff, motor_speed);
        prevPrintTimeMs = curTimeMs;
    }

}