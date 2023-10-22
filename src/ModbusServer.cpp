#include <ESP8266WiFi.h>

#include <ModbusIP_ESP8266.h>

#include "Global.h"
#include "ModbusServer.h"
#include "Ws2812.h"

// https://github.com/emelianov/modbus-esp8266/blob/master/examples/TCP-ESP/README.md

const uint16_t HREG_0 = 0; // Modbus Hreg Offset
const uint16_t HREG_1 = 1; // Modbus Hreg Offset
const uint16_t HREG_2 = 2; // Modbus Hreg Offset

// Modbus Registers Offsets
const uint16_t LED_COIL = 0;
bool coil = true;

// ModbusIP object
ModbusIP mb;

// Callback function for client connect. Returns true to allow connection.
bool cbConnect(IPAddress ip)
{
    Serial.print("Modbus Client Connected: ");
    Serial.println(ip);
    return true;
}

bool cbDisconnect(IPAddress ip)
{
    Serial.print("Modbus Client Disconnect: ");
    Serial.println(ip);
    return true;
}
void cbGetCoil(uint16_t coil)
{
    Serial.print("Modbus Client GetCoil: ");
    Serial.println(coil);
    return;
}

bool cb(Modbus::ResultCode event, uint16_t transactionId, void *data)
{ // Callback to monitor errors
    if (event != Modbus::EX_SUCCESS)
    {
        Serial.print("Request result: 0x");
        Serial.print(event, HEX);
    }
    return true;
}

void modbusServerSetup()
{
    int port = Assembly.cfg.modbus[Assembly.cfg.index].port;
    Serial.print("Modbus Server starting on port: ");
    Serial.println(port);
    
    mb.server(port); // Act as Modbus TCP server
    mb.onConnect(cbConnect);
    // mb.onDisconnect(cbDisconnect);
    // mb.onGetCoil(cbGetCoil);
    mb.addHreg(HREG_0, 111, 1);  // Expose Holding Register #0
    mb.addHreg(HREG_1, 0xFF, 1); // Expose Holding Register #1
    mb.addHreg(HREG_2, 200, 1);  // Expose Holding Register #2
    mb.addCoil(LED_COIL, 1);
    mb.addCoil(1, 1);
    mb.addCoil(2, 1);
}

void modbusServerLoop()
{
    mb.task(); // Common local Modbus task
    /*
    mb.readHreg(0, HREG_0, &Assembly.modbus.holdingRegister[HREG_0], 1, cb);
    mb.readHreg(0, HREG_1, &Assembly.modbus.holdingRegister[HREG_1], 1, cb);
    mb.readHreg(0, HREG_2, &Assembly.modbus.holdingRegister[HREG_2], 1, cb);
*/

    Assembly.modbus.holdingRegister[HREG_0] = mb.Hreg(HREG_0);
    Assembly.modbus.holdingRegister[HREG_1] = mb.Hreg(HREG_1);
    Assembly.modbus.holdingRegister[HREG_2] = mb.Hreg(HREG_2);

    Assembly.modbus.coils[LED_COIL] = mb.Coil(LED_COIL);
    Assembly.modbus.coils[1] = mb.Coil(1);
    Assembly.modbus.coils[2] = mb.Coil(2);

    if (Assembly.modbus.coils[LED_COIL] != coil)
    {
        coil = Assembly.modbus.coils[LED_COIL];
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