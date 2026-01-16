#include "water_level.h"
#include <Preferences.h>
#include <Arduino.h>

const int TRIGGER_PIN = 5;
const int ECHO_PIN = 4;

Preferences tankPrefs;
int tankEmptyDist = 100;
int tankFullDist = 10;


void setupWaterLevelSensor() {
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

int saveTankSettings(int e, int f) {
    if (e < 0) return -1;
    if (f < 0) return -1;  
    if (e <= f) return -1;
    tankEmptyDist = e;
    tankFullDist = f;
    
    tankPrefs.begin("tank", false);
    tankPrefs.putInt("empty", e);
    tankPrefs.putInt("full", f);
    tankPrefs.end();
    return 0;
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

int readWaterLevelPercent(int emptyDist, int fullDist) {
    int distanceCm = getDistnaceCm();
    if (distanceCm == -1) {
        return -1; 
    }

    if (distanceCm >= emptyDist) {
        return 100; 
    } else if (distanceCm <= fullDist) {
        return 0; 
    } else {
        return map(distanceCm, fullDist, emptyDist, 0, 100);
    }
}

