#ifndef AUTO_MANAGER_H
#define AUTO_MANAGER_H

extern bool isAutoMode;
extern int autoThreshold;
extern int minWaterLevelPercent;            

void initAutoManager();     
void runAutoLogic();        
void setAutoMode(bool mode); 
void setAutoThreshold(int threshold);
void setMinWaterLevel(int percent);

#endif