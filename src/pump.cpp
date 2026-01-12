#include "pump.h"

unsigned long pumpOffTime = 0;
bool isPumpActive = false;

void setupPump() {
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW); 
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
    }
}