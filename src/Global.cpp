
#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "Global.h"
#include "credentials.h"

// global object definition
clAssembly Assembly;

// Use http://arduinojson.org/v6/assistant to compute the capacity.
StaticJsonDocument<2048> doc;

void SerialFileOut(const char *filename)
{

    File file = LittleFS.open(filename, "r"); // Open the file
    // print out filecontent
    Serial.println(String("Assembly.setup --> configfile: ") + filename + " found... size:" + file.size());

    while (file.available())
    {
        Serial.write(file.read());
    }
    Serial.println();
    file.close(); // Close the file again
}

#define DEVICEID "Assembly-001"
void clAssembly::setupDevice()
{
    char filename[] = "/config_main.json";

    if (LittleFS.exists(filename))
    {
        // SerialFileOut(filename);

        File file = LittleFS.open(filename, "r"); // Open the file again

        // parse JSON
        DeserializationError error = deserializeJson(doc, file);

        // Test if parsing succeeds.
        if (error)
        {
            Serial.print(F("Assembly.setupDevice --> deserializeJson() failed: "));
            Serial.println(error.f_str());
        }

        // Serial.println(String("Assembly.setup --> configfile number of entries: ") + doc.size());

        strncpy(deviceId, doc["DEVICEID"] | DEVICEID, sizeof(deviceId));
        cfg.espMode = doc["ESPMODE"] | ESPMODE_MODBUS;
        cfg.publisherInterval = doc["PUBLISHERINTERVAL"] | 5000;
        cfg.accessPointEnabled = doc["ACCESSPOINT"];
        cfg.jsEnabled = doc["JSENABLE"];

        Serial.println(String("Assembly.setupDevice --> deviceid: ") + deviceId);
        Serial.println(String("Assembly.setupDevice --> espMode: ") + cfg.espMode);
        Serial.println(String("Assembly.setupDevice --> publisherInterval: ") + cfg.publisherInterval);
        Serial.println(String("Assembly.setupDevice --> accesspoint_enable: ") + cfg.accessPointEnabled);
        Serial.println(String("Assembly.setupDevice --> js_enable: ") + cfg.jsEnabled);

        file.close(); // Close the file again
    }
    else
    {
        Serial.println(String("Assembly.setupDevice --> error: NO ") + filename + " found, works with default defines.");
        strcpy(deviceId, DEVICEID);
        cfg.accessPointEnabled = true;
    }
}

void clAssembly::setupWifi()
{
    char filename[] = "/config_wlan.json";

    if (LittleFS.exists(filename))
    {
        // SerialFileOut(filename);

        File file = LittleFS.open(filename, "r"); // Open the file again

        // parse JSON
        DeserializationError error = deserializeJson(doc, file);

        // Test if parsing succeeds.
        if (error)
        {
            Serial.print(F("Assembly.setupWifi --> deserializeJson() failed: "));
            Serial.println(error.f_str());
        }

        // assigne values
        // get array size

        // Serial.println(String("Assembly.setup --> configfile number of entries: ") + doc.size());
        byte i = 0;
        for (JsonObject item : doc.as<JsonArray>())
        {
            if (i < (sizeof(cfg.wifi) / sizeof(cfg.wifi[0])))
            {
                strncpy(cfg.wifi[i].ssid, item["SSID"] | WIFI_SSID, sizeof(cfg.wifi[i].ssid));
                strncpy(cfg.wifi[i].pw, item["PASSWORD"] | WIFI_PASSWORD, sizeof(cfg.wifi[i].pw));

                Serial.println(String("Assembly.setupWifi --> entry: ") + i + " / " + cfg.wifi[i].ssid + " / " + cfg.wifi[i].pw);
            }

            i++;
        }

        file.close(); // Close the file again
    }
    else
    {
        Serial.println(String("Assembly.setupWifi --> error: NO ") + filename + " found, works with default defines.");
        strcpy(cfg.wifi[0].ssid, WIFI_SSID);
        strcpy(cfg.wifi[0].pw, WIFI_PASSWORD);
        strcpy(cfg.wifi[1].ssid, WIFI_SSID_1);
        strcpy(cfg.wifi[1].pw, WIFI_PASSWORD_1);
        strcpy(cfg.wifi[2].ssid, "");
        strcpy(cfg.wifi[2].pw, "");
    }
}

void clAssembly::setupModbus()
{
    char filename[] = "/config_modbus.json";

    if (LittleFS.exists(filename))
    {
        // SerialFileOut(filename);

        File file = LittleFS.open(filename, "r"); // Open the file again

        // parse JSON
        DeserializationError error = deserializeJson(doc, file);

        // Test if parsing succeeds.
        if (error)
        {
            Serial.print(F("Assembly.setupMqtt --> deserializeJson() failed: "));
            Serial.println(error.f_str());
        }

        // assigne values
        // get array size

        // Serial.println(String("Assembly.setup --> configfile number of entries: ") + doc.size());
        byte i = 0;
        for (JsonObject item : doc.as<JsonArray>())
        {
            if (i < (sizeof(cfg.modbus) / sizeof(cfg.modbus[0])))
            {
                cfg.modbus[i].port = item["PORT"] | MODBUS_PORT;
            }

            i++;
        }

        file.close(); // Close the file again
    }
    else
    {
        Serial.println(String("Assembly.setupModbus --> error: NO ") + filename + " found, works with default defines.");
        cfg.modbus[0].port = MODBUS_PORT;

        cfg.modbus[1].port = MODBUS_PORT;

        cfg.modbus[2].port = MODBUS_PORT;
    }
}

// read configuration from file
void clAssembly::setup()
{
    Serial.println("Assembly.setup --> begin");

    // compile date
    compileDate = String(CPP_VERSION) + __TIMESTAMP__;
    Serial.print("Assembly.setup --> compile date: ");
    Serial.println(compileDate);

    if (!LittleFS.begin())
    {
        Serial.println("Assembly.setup --> An Error has occurred while mounting LittleFS");
        delay(1000);
    }

    setupDevice();
    setupWifi();
    setupModbus();

    Serial.println("Assembly.setup --> end");
}

void clAssembly::newProcess(const char *j)
{
    job = j;
    processState = 1;
    changedState = true;
}
void clAssembly::startProcess()
{
    processState = 11;
    changedState = true;
}

void clAssembly::finishProcess()
{
    processState = 12;
    changedState = true;
}

void clAssembly::abortProcess()
{
    processState = 13;
    changedState = true;
}
void clAssembly::wlanConnectedProcess()
{
    processState = 21;
    changedState = true;
}
void clAssembly::rebootProcess()
{
    processState = 31;
    changedState = true;
}

// gettter will reset flag
bool clAssembly::getChangeState()
{
    bool returnValue = changedState;
    changedState = false;
    return returnValue;
}

void clAssembly::setProcessState(String state)
{
    if (state == "new")
    {
        processState = 1;
        changedState = true;
    }
    else if (state == "started")
    {
        processState = 11;
        changedState = true;
    }
    else if (state == "finished")
    {
        processState = 12;
        changedState = true;
    }
    else if (state == "aborted")
    {
        processState = 13;
        changedState = true;
    }
    else if (state == "wlanOk")
    {
        processState = 21;
        changedState = true;
    }
    else if (state == "reboot")
    {
        processState = 31;
        changedState = true;
    }
    else
    {
        processState = 99;
        changedState = true;
    }
}


String clAssembly::getProcessState()
{
    if (processState == 0)
    {
        return String("null");
    }
    else if (processState == 1)
    {
        return String("new");
    }
    else if (processState == 11)
    {
        return String("started");
    }
    else if (processState == 12)
    {
        return String("finished");
    }
    else if (processState == 13)
    {
        return String("aborted");
    }
    else if (processState == 21)
    {
        return String("wlanOk");
    }
    else if (processState == 31)
    {
        return String("reboot");
    }
    else if (processState == 99)
    {
        return String("undef:") + processState;
    }
    else
    {
        return String("boot");
    }
}

