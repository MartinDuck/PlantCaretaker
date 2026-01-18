#include "web_handlers.h"
#include <LittleFS.h>
#include "moisture_sensor.h"
#include "pump.h" 
#include "water_level.h"
#include <Preferences.h>
#include "auto_manager.h"

extern int moistureLevel;
extern unsigned long lastWateringTimestamp;
extern int tankEmptyDist;
extern int tankFullDist;
extern int waterLevelPercent;
extern int saveTankSettings(int e, int f);

void setupWebRoutes(AsyncWebServer* server) {
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });

    server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/style.css", "text/css");
    });

    server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/script.js", "application/javascript");
    });

    server->on("/get_soil", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", String(moistureLevel) + "%");
    });

    server->on("/get_raw", HTTP_GET, [](AsyncWebServerRequest *request){
        int val = readRaw();
        request->send(200, "text/plain", String(val));
    });

    server->on("/measure_distance", HTTP_GET, [](AsyncWebServerRequest *request){

    int rawDist = getDistanceCm(); 
    
    if (rawDist > 0 && rawDist < 400) {
        request->send(200, "text/plain", String(rawDist));
    } else {
        request->send(200, "text/plain", "-1"); 
    }
    });

    server->on("/pump", HTTP_GET, [](AsyncWebServerRequest *request){

        if(isAutoMode) {
            request->send(403, "text/plain", "Auto mode is active. Cannot manually start pump.");
            return;
        }
        if (blockUntillMillis > millis()) {
            request->send(403, "text/plain", "Pump is in cooldown period. Please wait.");
            return;
        }
        startPump(3000);
        request->send(200, "text/plain", "Pump activated for 3 seconds.");
    });

    server->on("/last_watering", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", String(lastWateringTimestamp));

    });

    server->on("/get_water_level", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", String(waterLevelPercent));
    });

    server->on("/get_full_config", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = "{";
        json += "\"empty\":" + String(tankEmptyDist) + ",";
        json += "\"full\":" + String(tankFullDist) + ",";
        json += "\"minWater\":" + String(minWaterLevelPercent);
        json += ",\"autoMode\":" + String(isAutoMode ? "true" : "false");
        json += ",\"threshold\":" + String(autoThreshold);
        json += "}";
        request->send(200, "application/json", json);
    });

    

    server->on("/set_tank", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("empty") && request->hasParam("full")) {
        int e = request->getParam("empty")->value().toInt();
        int f = request->getParam("full")->value().toInt();
        int minWater = request->getParam("minWater")->value().toInt();

        setMinWaterLevel(minWater);
        int result = saveTankSettings(e, f); 
        if (result != 0) {
            request->send(400, "text/plain", "Nieprawidłowe wartości");
            return;
        }
        waterLevelPercent = readWaterLevelPercent(tankEmptyDist, tankFullDist);
        request->send(200, "text/plain", "OK");
    } else {
        request->send(400, "text/plain", "Brak parametrow");
    }
    });

    server->on("/set_auto", HTTP_GET, [](AsyncWebServerRequest *request){
        if (request->hasParam("state")) {
            String state = request->getParam("state")->value();

            if (state == "1") {
                setAutoMode(true);
            } else {
                setAutoMode(false);
            }

            Preferences prefs;
            prefs.begin("garden", false);
            prefs.putBool("auto", isAutoMode);
            prefs.end();

            request->send(200, "text/plain", "OK");
        }
    });
    
    server->on("/set_threshold", HTTP_GET, [](AsyncWebServerRequest *request){
        if (request->hasParam("val")) {
            int value = request->getParam("val")->value().toInt();
            setAutoThreshold(value);
            request->send(200, "text/plain", "OK");
        }
    });


    
}