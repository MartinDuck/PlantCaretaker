#include "auto_manager.h"
#include "water_level.h"
#include "pump.h" 

bool isAutoMode = false;
int autoThreshold = 30;
unsigned long nextAllowedWatering = 0;
extern int moistureLevel;
extern int waterLevelPercent;

void runAutoLogic() {
    return;
}

void setAutoMode(bool mode) {
    isAutoMode = mode;
}