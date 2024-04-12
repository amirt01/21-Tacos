//
// Created by Arya Goutam on 4/2/2024
//

#ifndef SMARTCYCLE_INCLUDE_ENCODER_HPP_
#define SMARTCYCLE_INCLUDE_ENCODER_HPP_

#include "Arduino.h"

template<int enc_a_pin, int enc_b_pin>
class Encoder{
    int encoder_val = 0;
    bool enc_a_last = false;

    public:
    Encoder() {
        pinMode(enc_a_pin, INPUT);
        pinMode(enc_b_pin, INPUT);
    }

    void loop() {
        bool enc_a = !digitalRead(enc_a_pin);
        bool enc_b = !digitalRead(enc_b_pin);
        if(enc_a != enc_a_last){
            if(enc_b != enc_a){ //CW
                encoder_val += 1;
            } else { //CCW
                encoder_val -= 1;
            }
            enc_a_last = enc_a;
        }
    }

    void set_enc_val(int new_encoder_val){
        encoder_val = new_encoder_val;
    }

    int get_enc_val(){
        return encoder_val;
    }
};



#endif // SMARTCYCLE_INCLUDE_ENCODER_HPP_