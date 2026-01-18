#ifndef AUTO_MANAGER_H
#define AUTO_MANAGER_H

extern bool isAutoMode;
extern int autoThreshold;

void initAutoManager();     
void runAutoLogic();        
void setAutoMode(bool mode); 

#endif