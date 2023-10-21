/*
 * Arduino Interface with TinyJS Load Funktions from FileSystem
 */

#include <Arduino.h>
#include <LittleFS.h>

#include "ArduinoTinyJs.h"

void ArduinoTinyJs::loadSetup()
{
  load(&initStr, "init-app.js");
}

void ArduinoTinyJs::loadLoop()
{
  load(&cyclicStr, "cyclic-app.js");
}

void ArduinoTinyJs::loadTearDown()
{
  load(&exitStr, "exit-app.js");
}

void ArduinoTinyJs::load(String *code, const char *filename)
{

  if (LittleFS.exists(filename))
  {
    // SerialFileOut(filename);

    File file = LittleFS.open(filename, "r"); // Open the file again

    *code = file.readString();

    file.close(); // Close the file again
  }
  else
  {
    String s;
        s = String("Load TinyJS File --> error: ") + filename + " not found.";
        Serial.println(s);
        errorStr = s;
        *code = String("print('") + s + "');";
  }
}
