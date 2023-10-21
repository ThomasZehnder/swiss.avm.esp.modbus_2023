#include "Global.h" //doc, Assembly

#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>

#include <ESP8266HTTPClient.h>
#include "HttpRestClient.h"
#include "HttpServer.h"

extern ESP8266WiFiMulti wifiMulti;
#include <WiFiClientSecure.h>

int counterX = 0;

// compose message to send to cloud
String getHttpRestRequest()
{
    // see: https://arduinojson.org/
    doc.clear();

    doc["hostname"] = WiFi.hostname();
    doc["device_id"] = Assembly.deviceId;
    // doc["compiledate"] = Assembly.compileDate;

    counterX = (counterX + 1) % 100;
    // doc["counter"] = counterX;

    doc["milli"] = millis();
    // doc["jitter"] = millis() % 1000;
    // doc["rssi"] = httpRssi();
    doc["localIp"] = Assembly.localIp;

    
    doc["key_1"] = Assembly.keys[0].pressed;
    doc["key_2"] = Assembly.keys[1].pressed;
    doc["key_3"] = Assembly.keys[2].pressed;

    doc["key_cnt_1"] = Assembly.keys[0].pressedCounter;
    doc["key_cnt_2"] = Assembly.keys[1].pressedCounter;
    doc["key_cnt_3"] = Assembly.keys[2].pressedCounter;

    doc["processState"] = Assembly.getProcessState();
    
    // Lastly, you can print the resulting JSON to a String, use the "pretty" variant for readable outputs in browser
    String output;
    serializeJson(doc, output);
    return (output);
}
