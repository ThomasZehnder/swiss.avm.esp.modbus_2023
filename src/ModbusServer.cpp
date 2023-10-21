#include <ESP8266WiFi.h>

#include <ModbusIP_ESP8266.h>

#include "ModbusServer.h"
#include "Ws2812.h"

const uint16_t REG = 512; // Modbus Hreg Offset
// Modbus Registers Offsets
const int LED_COIL = 100;
bool coil;

// ModbusIP object
ModbusIP mb;

// Callback function for client connect. Returns true to allow connection.
bool cbConn(IPAddress ip)
{
    Serial.print("Modbus Client Connected: ");
    Serial.println(ip);
    return true;
}

void modbusServerSetup()
{
    mb.server(503); // Act as Modbus TCP server
    mb.onConnect(cbConn);
    mb.addHreg(100); // Expose Holding Register #100
    mb.addCoil(LED_COIL);
}

void modbusServerLoop()
{
    mb.task(); // Common local Modbus task

    bool temp = mb.Coil(LED_COIL);
    if (temp != coil)
    {
        coil = temp;
        Serial.print("Modbus coil 100 changed: ");
        Serial.println(coil);
        if (coil)
        {
            setLed(2, "blue");
        }
        else
        {
            setLed(2, "red");
        }
    }
}