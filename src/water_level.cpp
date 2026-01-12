#include "water_level.h"

#include <Arduino.h>

const int TRIGGER_PIN = 5;
const int ECHO_PIN = 4;

void setupWaterLevelSensor() {
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

int getDistnaceCm() {
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000); 

    int distanceCm = duration / 58;

    if (distanceCm == 0 || distanceCm > 400) {
        return -1; 
    }

    return distanceCm;
}