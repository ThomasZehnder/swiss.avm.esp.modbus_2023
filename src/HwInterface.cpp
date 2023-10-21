#include <Arduino.h>
#include <HwInterface.h>

#include "Global.h"
#include "Ws2812.h"

// interval at which to blink (milliseconds)
const long SECOUND_INTERVAL = 1000;
unsigned long preSecoundMillis = 0;
bool secoundTick = false;

// 50ms tick (milliseconds)
const long CENTI_SECOUND_INTERVAL = 50;
unsigned long preCentiSecoundMillis = 0;
bool centiSecoundTick = false;

unsigned long currentMillis = 0;

// ledState used to set the LED
int ledState = LOW;

bool hwCentiSecoundTick(void)
{
    return centiSecoundTick;
}
bool hwSecoundTick(void)
{
    return secoundTick;
}

unsigned long hwGetMillis(void)
{
    return currentMillis;
}

void hwSetup(void)
{
    pinMode(TOGGLE_LED_PIN, OUTPUT);

    // for stromfresser
    // pinMode(D8, OUTPUT);
    // digitalWrite(D8, 1);

    Assembly.keys[0].pin = KEY1_PIN;
    Assembly.keys[1].pin = KEY2_PIN;
    Assembly.keys[2].pin = KEY3_PIN;

    int i = 0;
    for (i = 0; i < 3; i++)
    {
        pinMode(Assembly.keys[i].pin, INPUT_PULLUP);
    }
}

void hwLoop(void)
{
    // Stromfresser
    // digitalWrite(D8, (currentMillis % 30000) > 25000);

    currentMillis = millis();
    // secound tick
    if ((long)(currentMillis - preSecoundMillis) > 0)
    {
        // Blink slow 
        preSecoundMillis += Assembly.cfg.publisherInterval;

        secoundTick = true;

        // set the LED with the ledState of the variable:
        ledState = !ledState;
        digitalWrite(TOGGLE_LED_PIN, ledState);
    }
    else
    {
        secoundTick = false;
    }
    // 50ms tick
    if ((long)(currentMillis - preCentiSecoundMillis) > 0)
    {
        preCentiSecoundMillis = currentMillis + CENTI_SECOUND_INTERVAL;
        centiSecoundTick = true;
    }
    else
    {
        centiSecoundTick = false;
    }
}

// getter
bool keyEdge(int keyNumber)
{
    return Assembly.keys[keyNumber].edge;
}
int keyPressedCounter(int keyNumber)
{
    return Assembly.keys[keyNumber].pressedCounter;
}

void pollKeyPressed(void)
{
    int i = 0;
    for (i = 0; i < 3; i++)
    {
        bool pressed = digitalRead(Assembly.keys[i].pin);
        if ((pressed == false) && (pressed != Assembly.keys[i].pressed))
        {
            Assembly.keys[i].edge = true;
            Assembly.keys[i].pressedCounter++;
        }
        else
        {
            Assembly.keys[i].edge = false;
        }
        Assembly.keys[i].pressed = pressed;
    }
}


