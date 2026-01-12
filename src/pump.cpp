#include "pump.h"
#include <Preferences.h>
#include <time.h>

unsigned long pumpOffTime = 0;
bool isPumpActive = false;
unsigned long blockUntillMillis = 0;

Preferences prefs;
unsigned long lastWateringTimestamp = 0; 

void setupPump() {
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW); 

    prefs.begin("garden", false); 
    lastWateringTimestamp = prefs.getULong("lastTime", 0);
    prefs.end();
}

void startPump(int durationMs) {
    digitalWrite(PUMP_PIN, HIGH);
    pumpOffTime = millis() + durationMs;
    isPumpActive = true;
}

void updatePump() {
    if (isPumpActive && millis() >= pumpOffTime) {
        digitalWrite(PUMP_PIN, LOW);
        isPumpActive = false;
        blockUntillMillis = millis() + 30000;

        time_t now;
        time(&now);
        lastWateringTimestamp = (unsigned long)now;

        prefs.begin("garden", false);
        prefs.putULong("lastTime", lastWateringTimestamp);
        prefs.end();

        Serial.print("Zapisano czas podlewania do NVS: ");
        Serial.println(lastWateringTimestamp);

    }
}