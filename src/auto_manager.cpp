#include "auto_manager.h"
#include <Arduino.h>
#include <Preferences.h>
#include "pump.h"
#include "water_level.h"

extern int moistureLevel;
extern int waterLevelPercent;


bool isAutoMode = false;
int autoThreshold = 30; 
unsigned long nextAllowedWatering = 0;
int minWaterLevelPercent = 20;

const int BACKFLOW_OFFSET = 500; 
const unsigned long SOAKING_TIME = 15 * 60 * 1000; // 15 minutes 
const int K_PROPORTIONAL = 200; // 200ms for each percent below threshold

void runAutoLogic() {
    if (!isAutoMode || isPumpActive) return;

    unsigned long now = millis();

    if (now < nextAllowedWatering) return;
    if (now < blockUntillMillis) return;

    int error = autoThreshold - moistureLevel;

    if (error >= 5 && moistureLevel > 0) {
        
        waterLevelPercent = readWaterLevelPercent(tankEmptyDist, tankFullDist);
        if (waterLevelPercent > minWaterLevelPercent) {
        
            int doseTime = error * K_PROPORTIONAL;
            
            doseTime = constrain(doseTime, 1000, 5000) + BACKFLOW_OFFSET;

            Serial.print("AUTO: Wilgotność "); Serial.print(moistureLevel);
            Serial.print("%, Cel "); Serial.print(autoThreshold);
            Serial.print("%. Podaję dawkę: "); Serial.print(doseTime); Serial.println("ms");

            startPump(doseTime);
            nextAllowedWatering = now + SOAKING_TIME;
            
        } else {
            Serial.println("AUTO: Brak wody w zbiorniku");
           
        }
    }
}

void setAutoMode(bool mode) {
    isAutoMode = mode;
    
    Preferences prefs;
    prefs.begin("garden", false);
    prefs.putBool("auto", isAutoMode);
    prefs.end();
    
}

void setAutoThreshold(int threshold) {
    autoThreshold = constrain(threshold, 0, 100);
    
    Preferences prefs;
    prefs.begin("garden", false);
    prefs.putInt("threshold", autoThreshold);
    prefs.end();
}

void initAutoManager() {
    Preferences prefs;
    prefs.begin("garden", true);
    isAutoMode = prefs.getBool("auto", false);
    autoThreshold = prefs.getInt("threshold", 30);
    minWaterLevelPercent = prefs.getInt("minWater", 20);
    prefs.end();
}
void setMinWaterLevel(int level) {
    minWaterLevelPercent = constrain(level, 0, 100);
    
    Preferences prefs;
    prefs.begin("garden", false);
    prefs.putInt("minWater", minWaterLevelPercent);
    prefs.end();
}