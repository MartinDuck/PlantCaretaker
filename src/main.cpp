#include <Arduino.h>
#include <WiFiManager.h>     
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include "moisture_sensor.h"
#include "web_handlers.h"
#include "pump.h"
#include "water_level.h"
#include <Preferences.h>
#include "auto_manager.h"

AsyncWebServer server(8080);

int moistureLevel = 0;
int waterLevelPercent = 0;

void setup() {

    delay(1000); 
    Serial.begin(115200);

    setupSensor();
    setupPump();
    setupWaterLevelSensor();
    analogReadResolution(12); 
    loadTankSettings();
    initAutoManager();

    WiFiManager wm;

    wm.setConfigPortalTimeout(180);
    bool res = wm.autoConnect("SmartPlant_Config");

    if(!res) {
        Serial.println("Błąd połączenia");
        ESP.restart();
    }
    
    if (WiFi.getMode() & WIFI_AP) {
        Serial.println("Konfiguracja zakończona sukcesem");
        delay(2000);
        ESP.restart();
    }

    configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org", "time.google.com"); 

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    Serial.println("Startuję serwer");
    delay(1000);

    if (!MDNS.begin("caretaker")) {
        Serial.println("Błąd konfiguracji mDNS");
    } else {
        Serial.println("mDNS uruchomiony: http://caretaker.local:8080");
        MDNS.addService("http", "tcp", 8080);
    }

    if(!LittleFS.begin(true)){
        Serial.println("Blad LittleFS");
    }
    
    setupWebRoutes(&server);
    server.begin();
    Serial.println("Serwer uruchomiony");


    moistureLevel = readMoisturePercent(4095, 1500);
    waterLevelPercent = readWaterLevelPercent(tankEmptyDist, tankFullDist);
    Serial.print("Początkowy poziom wilgotności gleby: ");
    Serial.print(moistureLevel);
    Serial.println("%");
    Serial.print("Początkowy poziom wody w zbiorniku: ");
    Serial.print(waterLevelPercent);
    Serial.println("%");    
}


unsigned long previousMoistureMillis = 0;
unsigned long previousWaterLevelMillis = 0;
const long moisture_interval = 120000; // 2 minutes
const long water_level_interval = 3600000; // 1 hour



void loop() {

   if (updatePump()) {
        waterLevelPercent = readWaterLevelPercent(tankEmptyDist, tankFullDist);
    }

    unsigned long currentMillis = millis();

    if(blockUntillMillis > 0 && currentMillis >= blockUntillMillis) {
        moistureLevel = readMoisturePercent(4095, 1500);
        previousMoistureMillis = currentMillis;
        blockUntillMillis = 0;

    }

    if (currentMillis - previousMoistureMillis >= moisture_interval) {
        previousMoistureMillis = currentMillis;

        if (!isPumpActive && blockUntillMillis <= currentMillis) {
        
        moistureLevel = readMoisturePercent(4095, 1500); 
        Serial.print("Poziom wilgotności gleby: ");
        Serial.print(moistureLevel);
        Serial.println("%");
        }
    }

    if (currentMillis - previousWaterLevelMillis >= water_level_interval && !isPumpActive) {
        previousWaterLevelMillis = currentMillis;
        waterLevelPercent = readWaterLevelPercent(tankEmptyDist, tankFullDist);
        Serial.print("Poziom wody w zbiorniku: ");
        Serial.print(waterLevelPercent);
        Serial.println("%");
    }

    if (isAutoMode)
    {
        runAutoLogic();
    }
    
    


}

