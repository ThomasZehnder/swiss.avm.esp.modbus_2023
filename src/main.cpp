#include <Arduino.h>

#include "HttpServer.h"
#include "HttpRestClient.h"
#include "Wifi.h"
#include "HwInterface.h"
#include "Oled.h"
#include "Ws2812.h"


#include "Global.h"

#include "tiny-js/ArduinoTinyJs.h"

void setup()
{
    Serial.begin(115200);
    Serial.println();

    Assembly.setup(); // read config file

    ws2812Setup();
    oledSetup();
    fullColorWipe(Assembly.cfg.jsEnabled);

    // scanNetworks(); // call before httpSetup, show result in serial out
    oledShowNetworks(); // show result on OLED

    wifiSetup();

    httpServerSetup(); // will not longer block until WLAN connected


    hwSetup();

    if (Assembly.cfg.jsEnabled)
    {
        if (Assembly.cfg.espMode != ESPMODE_PROGRAMMING)
        {
            tinyJs.setup();
        }
        tinyJs.errorStr = "Start JS Interpreter first by click 'setup button' ...";
    }
    else
    {
        tinyJs.consoleStr[2] = "JS Disabled, enable in main config if required...";
        tinyJs.errorStr = "JS Disabled";
    }
    fullColorWipe(Assembly.cfg.jsEnabled);

}

void loop()
{

    hwLoop();

    // secound tick, if MQTT connected
    if (hwSecoundTick())
    {
        if (Assembly.getProcessState() == "boot")
        {
            ws2812Demo();
        }
    }

    // 50ms tick
    if (hwCentiSecoundTick())
    {
        oledLoop();
        ws2812Loop();

        pollKeyPressed();
    }

    if (Assembly.cfg.jsEnabled)
    {
        tinyJs.loop(); // default 500ms cyclic
    }

    wifiLoop();
    httpServerLoop();

}