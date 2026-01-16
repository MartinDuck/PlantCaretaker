#include "web_handlers.h"
#include <LittleFS.h>
#include "moisture_sensor.h"
#include "pump.h" 
#include "water_level.h"

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

    server->on("/pump", HTTP_GET, [](AsyncWebServerRequest *request){
        startPump(3000);
        request->send(200, "text/plain", "Pump activated for 3 seconds.");
    });

    server->on("/last_watering", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", String(lastWateringTimestamp));

    });

    server->on("/get_water_level", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", String(waterLevelPercent));
    });

    server->on("/get_tank_config", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = "{";
        json += "\"empty\":" + String(tankEmptyDist) + ",";
        json += "\"full\":" + String(tankFullDist);
        json += "}";
        request->send(200, "application/json", json);
    });

    server->on("/set_tank", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("empty") && request->hasParam("full")) {
        int e = request->getParam("empty")->value().toInt();
        int f = request->getParam("full")->value().toInt();
        
        int result = saveTankSettings(e, f); 
        if (result != 0) {
            request->send(400, "text/plain", "Nieprawidłowe wartości");
            return;
        }
        request->send(200, "text/plain", "OK");
    } else {
        request->send(400, "text/plain", "Brak parametrow");
    }
    });
    


    
}