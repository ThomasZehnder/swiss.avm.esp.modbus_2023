// Include the correct display library

// For a connection via I2C using the Arduino Wire include:
#include <Wire.h>        // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy: #include "SSD1306.h"

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, SDA, SCL); // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

#include "Oled.h"

#include "Global.h"
#include "credentials.h"

#include <ESP8266WiFi.h> //show Wifi connections
#include "Ws2812.h"
#include "tiny-js/ArduinoTinyJs.h"

// start pixel of blue area
#define Y_OFFSET 16
//"table line 1"
#define X_OFFSET_1 54

#define DEMO_DURATION 3000
typedef void (*Demo)(void);

int demoMode = 0;
int counter = 1;
void drawStartScreen();

void oledSetup()
{

    Serial.println("OledSetup --> Start");

    // Initialising the UI will init the display too.
    display.init();

    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);

    drawStartScreen();

    Serial.println("OledSetup --> End");
}
// helper
void drawMillisAsTime()
{
    String t;
    unsigned long ms = millis();
    unsigned long s = ms / 1000;
    unsigned long m = s / 60;
    unsigned long h = m / 60;

    ms = ms % 1000 / 100; // only 100ms resolution
    s = s % 60;
    m = m % 60;

    t = String(h, DEC) + ":" + String(m, DEC) + ":" + String(s, DEC) + ":" + String(ms, DEC);
    // Draw actual millis in h,m,s,ms top right
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 0, t); // top right
}

void drawStartScreen()
{
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, Assembly.deviceId);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0 + Y_OFFSET, "Search WiFi");
    display.drawString(0, 12 + Y_OFFSET, Assembly.cfg.wifi[0].ssid);
    display.drawString(0, 24 + Y_OFFSET, Assembly.cfg.wifi[1].ssid);
    display.drawString(0, 36 + Y_OFFSET, Assembly.cfg.wifi[2].ssid);
    display.display();
}

void drawRebootScreen()
{
    display.clear();
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, Assembly.deviceId);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0 + Y_OFFSET, "Reboot active");
    // display.setFont(ArialMT_Plain_24);
    // display.drawString(0, 16 + Y_OFFSET, "avm.swiss");

    int progress = (counter / 5) % 100;
    // draw the progress bar
    display.drawProgressBar(0, 32 + Y_OFFSET, 120, 10, progress);
    display.display();
}

void drawProgressScreen()
{

    // Draw a line horizontally
    display.drawHorizontalLine(0, Y_OFFSET - 1, 128); // last yellow

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, Assembly.deviceId);

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0 + Y_OFFSET, "Job working");

    int progress = (counter / 5) % 100;
    // draw the progress bar
    display.drawProgressBar(0, 14 + Y_OFFSET, 120, 10, progress);

    display.drawString(0, 24 + Y_OFFSET, "mqtt:" + Assembly.mqttBroker);

    // Draw Assembly State
    display.drawString(0, Y_OFFSET + 36, "Assembly State: " + Assembly.getProcessState());
}

void drawWifiOkScreen()
{
    // Draw a line horizontally
    display.drawHorizontalLine(0, Y_OFFSET - 1, 128); // last yellow

    // Draw actual MQTT status top right
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 0, String("MQTT:") + String("udef")); // top right

    // Draw ip
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, Assembly.localIp); // top right

    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0 + Y_OFFSET, "wait job, mqtt: ");

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 0 + Y_OFFSET, Assembly.mqttBroker);

    int progress = (counter / 5) % 100;
    // draw the progress bar
    display.drawProgressBar(0, 16 + Y_OFFSET, 120, 10, progress);

    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    // Draw Assembly State
    display.drawString(0, Y_OFFSET + 36, "Assembly State: ");
    display.drawString(X_OFFSET_1 + 24, Y_OFFSET + 36, Assembly.getProcessState());
}
void oledShowNetworks()
{
    display.clear();

    // Draw a line horizontally
    display.drawHorizontalLine(0, Y_OFFSET - 1, 128); // last yellow

    // Draw actual millis top right
    drawMillisAsTime();

    // Show Assembly Name
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, Assembly.deviceId); // top left
    display.setTextAlignment(TEXT_ALIGN_LEFT);

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("oledShowNetworks() -->scan done");
    if (n == 0)
    {
        display.drawString(0, Y_OFFSET, String("no networks found"));
    }
    else
    {
        for (int j = 0; j <= n / 4; j++)
        {
            Serial.println(String("oledShowNetworks() -->") + j);
            for (int i = 0; i < 4; ++i)
            {
                // Print SSID and RSSI for each network found
                String s("");
                s += j * 4 + i + 1;
                s += ": ";
                s += WiFi.SSID(j * 4 + i);
                s += " (";
                s += WiFi.RSSI(j * 4 + i);
                s += ")                                 ";

                display.drawString(0, Y_OFFSET + i * 12, s);
            }
            display.display();
            delay(1000);
            display.clear();
        }
    }
}

void drawAssemblyInfo()
{
    // Draw a line horizontally
    display.drawHorizontalLine(0, Y_OFFSET - 1, 128); // last yellow

    // Draw actual millis top right
    drawMillisAsTime();

    // Draw ip
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, Assembly.localIp); // top left

    // Draw Connection Status
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, Y_OFFSET, String("Wifi On: ") + String(Assembly.wifiConnected));
    
    // Job Name
    display.drawString(0, Y_OFFSET + 12, String("Job: ") + Assembly.job);
    // display.drawString(X_OFFSET_1, Y_OFFSET + 12, String(Assembly.job));

    // Draw Key Counter
    String s("Key [1,2,3]: ");
    s += String(Assembly.keys[0].pressedCounter);
    s += " ";
    s += String(Assembly.keys[1].pressedCounter);
    s += " ";
    s += String(Assembly.keys[2].pressedCounter);

    display.drawString(0, Y_OFFSET + 24, s);

    // Draw Key Counter
    display.drawString(0, Y_OFFSET + 36, "Assembly State: ");
    display.drawString(X_OFFSET_1 + 24, Y_OFFSET + 36, Assembly.getProcessState());
}

void drawModbusScreen()
{

    // Draw a line horizontally
    display.drawHorizontalLine(0, Y_OFFSET - 1, 128); // last yellow

    // Draw actual MQTT status top right
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 0, String("Wifi:") + String(Assembly.wifiConnected) ); // top right

    // Draw ip
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, Assembly.localIp); // top right

    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0 + Y_OFFSET, "Te: " + String("undef") + "°C " + "Hu: " + String("undef") + "%");

    // display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0 + Y_OFFSET + 12, "ldr: " + String("undef") + " raw: " + String("undef") + " : " + String("undef"));

    // display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0 + Y_OFFSET + 24, "noise: " + String("undef") + " raw: " + String("undef"));

    // display.setFont(ArialMT_Plain_10);
   display.drawString(0, 0 + Y_OFFSET + 36, "k1: " + String(Assembly.keys[0].pressedCounter) + " k2: " + String(Assembly.keys[1].pressedCounter) + " k3: " + String(Assembly.keys[2].pressedCounter));

}



void oledLoop()
{

    // reboot
    if (Assembly.cfg.espMode == 255)
    {
        drawRebootScreen();
    }
    else
    {

        // clear the display
        display.clear();

        // draw the current demo method
        if (Assembly.getProcessState() == "boot")
        {
            drawStartScreen();
        }
        else if (Assembly.getProcessState() == "reboot")
        {
            drawRebootScreen();
        }
        else if (Assembly.getProcessState() == "new")
        {
            drawAssemblyInfo();
        }
        else if (Assembly.getProcessState() == "started")
        {
            drawProgressScreen();
        }
        else if (Assembly.getProcessState() == "wlanOk")
        {
            // drawWifiOkScreen();
            drawModbusScreen();
        }
        else
        {
            // drawAssemblyInfo();
            drawModbusScreen();
        }
        //
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_RIGHT);
        display.drawString(128, 54, String(millis() / 100 % 10)); // bottom right
        //  The coordinates define the right end of the text
        // display.setTextAlignment(TEXT_ALIGN_RIGHT);
        // display.drawString(128, 0, String(millis()));   //top right
        //  write the buffer to the display
        display.display();
    }
    counter++;
}
