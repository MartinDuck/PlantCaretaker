#include "web_handlers.h"
#include <LittleFS.h>
#include "Sensor.h" 


void setupWebRoutes(AsyncWebServer* server) {
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });

    server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/style.css", "text/css");
    });

    server->on("/get_soil", HTTP_GET, [](AsyncWebServerRequest *request){
        int val = readPercent(4095, 3000); 
        request->send(200, "text/plain", String(val));
    });

    server->on("/get_raw", HTTP_GET, [](AsyncWebServerRequest *request){
        int val = readRaw();
        request->send(200, "text/plain", String(val));
    });

    
}