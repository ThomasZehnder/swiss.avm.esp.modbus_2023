/*
 * TinyJS
 *
 *  ESP functions specialized for ESP IIoT board
 */

#include <Arduino.h>

#include "Global.h"
#include "Ws2812.h"

#include "ArduinoTinyJs.h"

#include "TinyJS_EspFunctions.h"

#include "Wifi.h"

// CScriptVar shortcut macro
// #define scIsBool(a) (c->getParameter(a)->isBool())
#define scIsInt(a) (c->getParameter(a)->isInt())
#define scIsDouble(a) (c->getParameter(a)->isDouble())
#define scIsString(a) (c->getParameter(a)->isString())
#define scGetBool(a) (c->getParameter(a)->getBool())
#define scGetInt(a) (c->getParameter(a)->getInt())
#define scGetDouble(a) (c->getParameter(a)->getDouble())
#define scGetString(a) (c->getParameter(a)->getString())
// #define scReturnBool(a) (c->getReturnVar()->setBool(a))
#define scReturnInt(a) (c->getReturnVar()->setInt(a))
#define scReturnDouble(a) (c->getReturnVar()->setDouble(a))
#define scReturnString(a) (c->getReturnVar()->setString(a))
#define scReturnUndefined() (c->getReturnVar()->setUndefined())

// Esp.SetLed(pixel, color); //set led pixel[0..3] with color [#rrggbb, off/black, red, green, yellow, blue, white/on]
void scEspSetLed(CScriptVar *c, void *)
{
    if (scIsInt("pixel") && scIsString("color"))
    {
        int i = scGetInt("pixel");
        String color = String(scGetString("color").c_str());

        setLed(i, color);
    }
    scReturnUndefined();
}

void scEspClearLed(CScriptVar *c, void *)
{
    strip.clear();
    strip.show();
    scReturnUndefined();
}

// key getter
void scEspKeyPressed(CScriptVar *c, void *)
{
    if (scIsInt("key"))
    {
        int key = scGetInt("key");
        if (key < (int)(sizeof(Assembly.keys) / sizeof(Assembly.keys[0])))
        {
            scReturnInt(Assembly.keys[key].pressed);
            return;
        }
    }
    scReturnUndefined();
}

void scEspKeyPressedCounter(CScriptVar *c, void *)
{
    if (scIsInt("key"))
    {
        int key = scGetInt("key");
        if (key < (int)(sizeof(Assembly.keys) / sizeof(Assembly.keys[0])))
        {
            scReturnInt(Assembly.keys[key].pressedCounter);
            return;
        }
    }
    scReturnUndefined();
}

// millis()
void scEspMillis(CScriptVar *c, void *)
{
    scReturnInt(millis());
}

// set cycle time of js interpreter
void scEspJsSetCycleTime(CScriptVar *c, void *)
{
    if (scIsInt("time"))
    {
        int time = scGetInt("time");
        if (time < 200)
        {
            tinyJs.setCycleTime = 200;
            scReturnInt(tinyJs.setCycleTime);
            return;
        }
        else if (time > 10000)
        {
            tinyJs.setCycleTime = 1000;
            scReturnInt(tinyJs.setCycleTime);
            return;
        }
        else
        {
            tinyJs.setCycleTime = time;
            scReturnInt(tinyJs.setCycleTime);
            return;
        }
    }
    scReturnUndefined();
}

// connection state
void scWifiConnected(CScriptVar *c, void *)
{
    scReturnInt(Assembly.wifiConnected);
}




void scAssemblySetProcessState(CScriptVar *c, void *)
{
    if (scIsString("state"))
    {
        String state = scGetString("state").c_str();

        Assembly.setProcessState(state);

        scReturnString(Assembly.getProcessState().c_str());
        return;
    }
    scReturnUndefined();
}

void scAssemblyGetProcessState(CScriptVar *c, void *)
{
    scReturnString(Assembly.getProcessState().c_str());
}

// ----------------------------------------------- Register Functions
void registerEspFunctions(CTinyJS *tinyJS)
{

    // --- ESP and Arduino functions ---
    tinyJS->addNative("function Esp.setLed(pixel, color)", scEspSetLed, 0);          // Set WS2812 Strip LED, i index of pixel, color
    tinyJS->addNative("function Esp.clearLed()", scEspClearLed, 0);                  // turn of all WS2812 Strip Leds
    tinyJS->addNative("function Esp.getKey(key)", scEspKeyPressed, 0);               // get key pressed stat
    tinyJS->addNative("function Esp.getKeyCounter(key)", scEspKeyPressedCounter, 0); // get key pressed counter stat

    tinyJS->addNative("function Esp.millis()", scEspMillis, 0); // return millisecouns of ESP

    tinyJS->addNative("function JS.setCycleTime(time)", scEspJsSetCycleTime, 0); // set JS cycle time [200..10'000ms]

    tinyJS->addNative("function Wifi.connected()", scWifiConnected, 0); // //return if Wifi is connected


    tinyJS->addNative("function Assembly.getProcessState()", scAssemblyGetProcessState, 0);      // get Assembly process state
    tinyJS->addNative("function Assembly.setProcessState(state)", scAssemblySetProcessState, 0); // set Assembly process state ["new","started","finished","aborted"]
}
