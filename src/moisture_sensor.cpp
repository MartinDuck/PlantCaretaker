#include "moisture_sensor.h"

void setupSensor() {
    pinMode(MOISTURE_SENSOR_POWER, OUTPUT);
    digitalWrite(MOISTURE_SENSOR_POWER, LOW);
    analogReadResolution(12); 
}

int readRaw() {
    digitalWrite(MOISTURE_SENSOR_POWER, HIGH); 
    delay(500); 
    long int sum = 0;
    const int samples = 10;
    for (int i = 0; i < samples; i++) {
        sum += analogRead(MOISTURE_SENSOR_SIGNAL);
        delay(10); 
    }

    int rawValue = sum / samples;

    digitalWrite(MOISTURE_SENSOR_POWER, LOW); 
    return rawValue;
}

int readPercent(int dryValue, int wetValue) {
    int rawValue = readRaw();

    if (rawValue >= dryValue) {
        return 0;
    } else if (rawValue <= wetValue) {
        return 100;
    } else {
        return map(rawValue, dryValue, wetValue, 0, 100);
    }
}