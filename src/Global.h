#ifndef GLOBAL_H
#define GLOBAL_H
// global object definition
#include <Arduino.h>
#include <ArduinoJson.h>

#define CPP_VERSION "V0.00.00 "

#define ESPMODE_MODBUS 0
#define ESPMODE_PROGRAMMING 1

struct tstCfgWifi
{
    char ssid[32];
    char pw[32];
};

struct tstCfgModBus
{
    int port = 502;
};

struct tstPublisher_old
{
    unsigned long startMillis;
};



#define NBRCONNECTIONS 3
struct tstCfg
{
    tstCfgWifi wifi[NBRCONNECTIONS];

    tstCfgModBus modbus[NBRCONNECTIONS];
    
    byte espMode = 0;

    long publisherInterval = 1000;

    bool accessPointEnabled = true;
    bool jsEnabled = false;

    byte index = 0;             // index used for http and mqtt send to cloud, depends on found WIFI Network
};

struct tstKey
{
    int pin;
    bool edge;
    bool pressed;
    int pressedCounter;
};

struct tstLed
{
    String color = "#------";
};

struct tstModbus
{
    bool coils[20];
    uint16_t holdingRegister[20];

};

class clAssembly
{
public:
    String compileDate;
    char deviceId[32];


    bool wifiConnected = false;

    String localIp;
    String ssid;
    String mqttBroker;

    struct tstKey keys[3];

    struct tstLed leds[4];

    struct tstModbus modbus;

    tstCfg cfg;


private:
    int processState = 0;
    bool changedState = false;

public:
    void setup();
    void newProcess(const char *);
    void startProcess();
    void finishProcess();
    void abortProcess();

    void rebootProcess();
    void wlanConnectedProcess();

    String job;
    void setProcessState(String state);
    String getProcessState();
    bool getChangeState();

private:
    void setupDevice();
    void setupWifi();
    void setupModbus();
};

extern clAssembly Assembly;

// Allocate a static JsonDocument
// Use https://arduinojson.org/v6/assistant to compute the capacity.
extern StaticJsonDocument<2048> doc;

#endif // GLOBAL_H