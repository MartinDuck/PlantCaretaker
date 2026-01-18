#include "water_level.h"
#include <Preferences.h>
#include <Arduino.h>

const int TRIGGER_PIN = 5;
const int ECHO_PIN = 4;

Preferences tankPrefs;
int tankEmptyDist = 20;
int tankFullDist = 5;


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
int getDistanceCm() {
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000); 

    int distanceCm = duration / 58;

    if (distanceCm == 0 || distanceCm > 400) {
        return 0; 
    }

    return distanceCm;
}

int readWaterLevelPercent(int emptyDist, int fullDist) {
    int distanceCm = getDistanceCm();

    if (distanceCm == -1 || distanceCm > 400) { 
        return -1; 
    }

    if (emptyDist <= fullDist) {
        return 0;
    }

    if (distanceCm >= emptyDist) {
        return 0; 
    } 
    if (distanceCm <= fullDist) {
        return 100; 
    }

    return map(distanceCm, emptyDist, fullDist, 0, 100);
}

void loadTankSettings() {
    Preferences prefs;
    prefs.begin("tank", true); 
    tankEmptyDist = prefs.getInt("empty", 20); 
    tankFullDist = prefs.getInt("full", 5);
    prefs.end();
}

