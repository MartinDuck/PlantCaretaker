#include "pump.h"
#include <Preferences.h>
#include <time.h>
#include "water_level.h"
#include "auto_manager.h"

unsigned long pumpOffTime = 0;
bool isPumpActive = false;
unsigned long blockUntillMillis = 0;
int milisBlock = 30000;
extern int waterLevelPercent;


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
    if(waterLevelPercent < minWaterLevelPercent) return;
    if (isPumpActive) return;
    if (blockUntillMillis > millis()) return;
    digitalWrite(PUMP_PIN, HIGH);
    pumpOffTime = millis() + durationMs;
    isPumpActive = true;
}

int updatePump() {
    if (isPumpActive && millis() >= pumpOffTime) {
        digitalWrite(PUMP_PIN, LOW);
        isPumpActive = false;
        blockUntillMillis = millis() + milisBlock;

        time_t now;
        time(&now);
        lastWateringTimestamp = (unsigned long)now;

        prefs.begin("garden", false);
        prefs.putULong("lastTime", lastWateringTimestamp);
        prefs.end();

        return 1;
    }
    return 0;
}