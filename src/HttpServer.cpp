#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

#include <ArduinoJson.h>

#include "HttpServer.h"

#include "credentials.h"

#include "tiny-js/ArduinoTinyJs.h"

#include "Global.h"
#include "Ws2812.h"

#include "dictionary.h"
extern Dictionary<String, String> SubscriptionList;

ESP8266WiFiMulti wifiMulti; // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer server(80);

File fsUploadFile; // a File object to temporarily store the received file

String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)
void handleFileUpload();                // upload a new file to the SPIFFS
void handleFileStore();                 // store content to file

#define ACTIVITY_LED_PIN LED_BUILTIN

long triggerActivityTime = 0;
long triggerRebootTime = 0;

String lastDownloadFilename = "-";

void triggerActivity()
{
  digitalWrite(ACTIVITY_LED_PIN, 0);
  triggerActivityTime = millis();
}

int httpRssi(void)
{
  return WiFi.RSSI();
}

String httpLocalIP(void)
{
  return WiFi.localIP().toString();
}

void scanNetworks()
{

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("scanNetworks -->  start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  // Serial.println("");
  // Get Current Hostname
  Serial.printf("Default hostname: %s\n", WiFi.hostname().c_str());
  Serial.println("scanNetworks -->  end");
}

void handleJson()
{
  triggerActivity();

  // see: https://arduinojson.org/

  doc.clear();
  doc["device_id"] = Assembly.deviceId;
  doc["millis"] = millis();
  doc["rssi"] = httpRssi();
  doc["accesspoint_ip"] = WiFi.softAPIP().toString();

  // Create the "digital" array
  JsonArray digitalValues = doc.createNestedArray("digital");
  for (int pin = 0; pin < 17; pin++)
  {
    // Read the digital input
    int value = digitalRead(pin);

    // Add the value at the end of the array
    digitalValues.add(value);
    // digitalValues.add(pin);
    // digitalValues.add("pin");
  }

  // Serial.print(F("Sending: "));
  // serializeJson(doc, Serial);
  // Serial.println();

  // Lastly, you can print the resulting JSON to a String
  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output);
}

void setAllowCors()
{
  server.sendHeader("Access-Control-Allow-Origin", "*"); // Replace '*' with your allowed domain
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
}

void assemblyJson()
{
  triggerActivity();

  // see: https://arduinojson.org/
  doc.clear();

  doc["hostname"] = WiFi.hostname();
  doc["deviceId"] = Assembly.deviceId;
  doc["espMode"] = Assembly.cfg.espMode;
  doc["accesspoint_enable"] = Assembly.cfg.accessPointEnabled;
  doc["localIp"] = Assembly.localIp;
  doc["ssid"] = Assembly.ssid;
  doc["compiledate"] = Assembly.compileDate;

  doc["millis"] = millis();
  doc["rssi"] = httpRssi();

  doc["wifiConnected"] = Assembly.wifiConnected;

  doc["js_set_cycle_time"] = tinyJs.setCycleTime;
  doc["js_execute_time"] = tinyJs.executeTime;
  doc["js_cyclic_counter"] = tinyJs.loopCounter;
  doc["js_cyclic_console"] = tinyJs.consoleStr[0];
  doc["js_cyclic_error"] = tinyJs.errorStr;

  doc["key_1"] = Assembly.keys[0].pressed;
  doc["key_2"] = Assembly.keys[1].pressed;
  doc["key_3"] = Assembly.keys[2].pressed;

  doc["key_cnt_1"] = Assembly.keys[0].pressedCounter;
  doc["key_cnt_2"] = Assembly.keys[1].pressedCounter;
  doc["key_cnt_3"] = Assembly.keys[2].pressedCounter;

  doc["processState"] = Assembly.getProcessState();

  doc["coil_0"] = Assembly.modbus.coils[0];
  doc["coil_1"] = Assembly.modbus.coils[1];
  doc["coil_2"] = Assembly.modbus.coils[2];

  doc["register_0"] = Assembly.modbus.holdingRegister[0];
  doc["register_1"] = Assembly.modbus.holdingRegister[1];
  doc["register_2"] = Assembly.modbus.holdingRegister[1];

  doc["cfg_index"] = Assembly.cfg.index;

  doc["wifi_0"] = Assembly.cfg.wifi[0].ssid;
  doc["wifi_1"] = Assembly.cfg.wifi[1].ssid;
  doc["wifi_2"] = Assembly.cfg.wifi[2].ssid;

  doc["modbus_port_0"] = Assembly.cfg.modbus[0].port;
  doc["modbus_port_1"] = Assembly.cfg.modbus[1].port;
  doc["modbus_port_2"] = Assembly.cfg.modbus[2].port;

  // Lastly, you can print the resulting JSON to a String, use the "pretty" variant for readable outputs in browser
  String output;
  serializeJsonPretty(doc, output);
  setAllowCors();
  server.send(200, "application/json", output);
}

void handleRoot()
{
  triggerActivity();
  handleFileRead("index.html");
  // server.send(200, "text/plain", "hello from esp8266!");
}

void handleNotFound()
{
  triggerActivity();
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += server.method();
  message += " : ";
  message += (server.method() == HTTP_GET) ? "GET" : (server.method() == HTTP_POST) ? "POST"
                                                 : (server.method() == HTTP_PATCH)  ? "PATCH"
                                                                                    : "seeNbr";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

void (*rebootFunc)(void) = 0; // declare reset function @ address 0

void reboot(void)
{
  if (!server.hasArg("bootmode"))
  {
    Serial.println("reboot argument 'bootmode' not found!!");
    server.send(500, "text/plain", "500: reboot ignored ");
    return;
  }
  String arg = server.arg("bootmode");
  if (arg == "espreboot")
  {
    handleFileRead("reboot.html");
    server.send(200, "text/plain", "reboot arduino in 1 second !!!");
    triggerRebootTime = millis();
    triggerActivity();
    // Assembly.rebootProcess();
    Assembly.cfg.espMode = 255;
    colorWipe(strip.Color(0, 0, 16), 100); // blue
    colorWipe(strip.Color(16, 0, 0), 100); // red
  }
  else
  {
    if (tinyJs.setCmd(arg))
    {
      Serial.print("tiny-js cmd set : ");
      Serial.println(arg);
      server.send(200, "text/plain", "tiny-js cmd done: " + arg);
    }
    else
    {
      Serial.print("unknown reboot : ");
      Serial.println(arg);
      server.send(500, "text/plain", "500: unknown argument: " + arg);
    }
  }
}

void success(void)
{
  triggerActivity();
  String msg = "<h1>Upload Result</h1>";
  msg += "Last uploaded file: " + lastDownloadFilename;
  msg += "<br><a href=\"/a-upload.html\">Upload next file.</a>";
  msg += "<br><a href=\"/dir\">Pure directory</a>";
  msg += "<br><a href=\"/\">Back to main page.</a>";
  server.send(200, "text/html", msg);
}

void dir(void)
{
  triggerActivity();

  if (Assembly.cfg.espMode == ESPMODE_PROGRAMMING)
  {
    server.send(200, "text/html", "Not available in ESP Programming Mode");
    Serial.println("/dir disabled...");
    return;
  }

  String msg = "directory root: <table><tr><th>FILE</th><th>SIZE</th></tr>";
  const char dirname[] = "/";
  Serial.printf("Listing directory: %s\n", dirname);

  Dir root = LittleFS.openDir(dirname);

  while (root.next())
  {
    File file = root.openFile("r");
    Serial.print(" FILE: ");
    Serial.print(root.fileName());
    Serial.print(" SIZE: ");
    char sz[200];
    ltoa(file.size(), sz, 10);
    msg += "<tr><td><a href=\"/" + root.fileName() + "\">" + root.fileName() + "</a> </td><td>" + sz + "</td></tr>";
    Serial.println(sz);
    file.close();
  }
  Serial.println("");

  msg += "</table>";
  server.send(200, "text/html", msg);
}
void subscriptionList(void)
{
  triggerActivity();

  String msg = "<pre>";
  for (byte i = 0; i < SubscriptionList.length(); i++)
  {
    msg += "Key: " + SubscriptionList.getKeyByIndex(i) + " Value: " + SubscriptionList.getByIndex(i) + "\n";
  }
  msg += "</pre>";
  server.send(200, "text/html", msg);
}

void jsConsole(void)
{
  triggerActivity();

  String msg = "";
  for (byte i = 0; i < tinyJs.consoleStrLength; i++)
  {
    msg += tinyJs.consoleStr[i] + "\n";
  }
  server.send(200, "text/html", msg);
}

void jsError(void)
{
  triggerActivity();
  String msg = "";
  msg += "cycle time: " + String(tinyJs.cycleTime) + "ms";
  msg += "; execute time: " + String(tinyJs.executeTime) + "ms";
  msg += "\n--> " + tinyJs.errorStr;
  server.send(200, "text/html", msg);
}
void _getKeys()
{
  triggerActivity();

  String output;

  // build manualy
  output += "{";
  output += "\"key_1\":" + String(Assembly.keys[0].pressed) + ",";
  output += "\"key_2\":" + String(Assembly.keys[1].pressed) + ",";
  output += "\"key_3\":" + String(Assembly.keys[2].pressed);
  output += "}";

  setAllowCors();
  server.send(200, "application/json", output);
}

void _setLedRgb(void)
{
  triggerActivity();
  String rgb = server.arg("color");
  setLed(0, rgb);

  String msg = "setLed(rgb):  " + rgb;
  setAllowCors();
  server.send(200, "text/html", msg);
  Serial.println(msg);
}

// usage: http://192.168.1.72/setleds?colors=%23402020,%23FFFFFF,%23FFFFFF,%23FFFFFF,blue,%23FFFFFF,red,blue,
// IMPORTANT: reading comma !!!
void _setLeds(void)
{
  triggerActivity();
  String input = server.arg("colors");
  // setLed(0, rgb);

  String rs = "";
  int parserCnt = 0;
  int rFromIndex = 0;
  int rToIndex = -1;
  while (8 >= parserCnt)
  {
    rFromIndex = rToIndex + 1;
    rToIndex = input.indexOf(',', rFromIndex);
    /*
    Serial.print(rToIndex);
    Serial.print("-->");
    */
    if (rToIndex == 0 || rToIndex == -1)
      break;
    rs = input.substring(rFromIndex, rToIndex);
    /*
    Serial.print(parserCnt);
    Serial.print("-->");
    Serial.println(rs);
    */
    setLed(parserCnt, rs);

    parserCnt++;
  }

  String msg = "setLeds(rgbs):  " + input;
  setAllowCors();
  server.send(200, "text/html", msg);
  Serial.println(msg);
}

void httpServerSetup(void)
{

  Serial.println("HttpSetup --> Start");

  pinMode(ACTIVITY_LED_PIN, OUTPUT);
  digitalWrite(ACTIVITY_LED_PIN, 1);

  for (byte i = 0; i < (sizeof(Assembly.cfg.wifi) / sizeof(Assembly.cfg.wifi[0])); i++)
  {
    wifiMulti.addAP(Assembly.cfg.wifi[i].ssid, Assembly.cfg.wifi[i].pw); // add Wi-Fi networks you want to connect to, see credentials.h
  }

  Serial.println("HttpSetup --> Connecting Wifi Multi part...");
  bool wifiConnectError = true;
  if (wifiMulti.run(2000) == WL_CONNECTED)
  {
    Serial.println("HttpSetup --> connected Wifi ...");
    wifiConnectError = false;
    // get Wifi Informations
    Serial.print("HttpSetup  --> IP address: ");
    Serial.println(WiFi.localIP());
    Assembly.wifiConnected = true;

    Assembly.wlanConnectedProcess();
  }

  if (Assembly.cfg.accessPointEnabled || wifiConnectError)
  {
    Serial.println("HttpSetup --> Setting the AP Mode with SSID, NO Password...");
    // Setting the AP Mode with SSID, Password, and Max Connection Limit
    if (WiFi.softAP(Assembly.deviceId, "", 1, false, 1) == true)
    {
      Serial.print("Access Point is Creadted with MAC ADDDR: ");
      Serial.println(WiFi.softAPmacAddress());
      Serial.print("Access Point IP: ");
      Serial.println(WiFi.softAPIP());
      Serial.print("getFlashChipId: ");
      Serial.println(ESP.getFlashChipId());
    }
    else
    {
      Serial.println("Unable to Create Access Point");
    }
  }

  Serial.println("HttpSetup --> Mound File System: LittleFS");
  // Start the SPI Flash Files System
  if (!LittleFS.begin())
  {
    Serial.println("HttpSetup --> An Error has occurred while mounting LittleFS");
    delay(1000);
  }

  // Setup Http Server
  server.on("/", handleRoot);
  server.on("/json", handleJson); // test only
  server.on("/assembly", assemblyJson);

  server.on("/inline", []()
            { server.send(200, "text/plain", "this works as well"); });

  server.on("/reboot", reboot);

  server.on("/success", success);
  server.on("/dir", dir);

  server.on("/subscriptionList", subscriptionList);

  server.on("/jsconsole", jsConsole);
  server.on("/jserror", jsError);

  server.on("/setled", _setLedRgb);

  server.on("/setleds", _setLeds);

  server.on("/getkeys", _getKeys);

  server.on("/upload", HTTP_GET, []() { // if the client requests the upload page
    triggerActivity();
    if (!handleFileRead("/a-upload.html"))              // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });

  server.on(
      "/upload", HTTP_POST, // if the client posts to the upload page
      []()
      { server.send(200); }, // Send status 200 (OK) to tell the client we are ready to receive
      handleFileUpload       // Receive and save the file
  );

  server.on("/store", HTTP_POST, handleFileStore);

  // server.onNotFound(handleNotFound);
  server.onNotFound([]() {             // If the client requests any URI
    if (!handleFileRead(server.uri())) // send it if it exists
      handleNotFound();                // otherwise, respond with a 404 (Not Found) error
  });

  server.begin();
  Serial.println("HttpSetup --> HTTP server started...");

  Serial.println("HttpSetup --> End");
}

void httpServerLoop(void)
{
  // connect to strongest wifi, and reconnect
  wifiMulti.run();

  server.handleClient();

  if (triggerActivityTime != 0)
  {
    if ((long)millis() - triggerActivityTime - 200 > 0)
    {
      digitalWrite(ACTIVITY_LED_PIN, 1);
      triggerActivityTime = 0;
    }
  }
  // reboot after 2000ms
  if (triggerRebootTime != 0)
  {
    if ((long)millis() - triggerRebootTime - 2000 > 0)
    {
      Serial.println("httpLoop --> Reboot REST service");
      delay(50);    // allow serial output to finish
      rebootFunc(); // call reboot ESP.restart()
    }
  }
}

String getContentType(String filename)
{ // convert the file extension to the MIME type
  if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".pdf"))
    return "application/pdf";
  else if (filename.endsWith(".png"))
    return "image/png";
  else if (filename.endsWith(".gif"))
    return "image/gif";
  else if (filename.endsWith(".svg"))
    return "image/svg+xml";
  else if (filename.endsWith(".jpg"))
    return "image/jpeg";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  else if (filename.endsWith(".json"))
    return "application/json";
  return "text/plain";
}

bool handleFileRead(String path)
{ // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/"))
    path += "index.html";                    // If a folder is requested, send the index file
  String contentType = getContentType(path); // Get the MIME type
  String pathWithGz = path + ".gz";
  if (LittleFS.exists(pathWithGz) || LittleFS.exists(path))
  {                                                     // If the file exists, either as a compressed archive, or normal
    if (LittleFS.exists(pathWithGz))                    // If there's a compressed version available
      path += ".gz";                                    // Use the compressed verion
    File file = LittleFS.open(path, "r");               // Open the file
    size_t sent = server.streamFile(file, contentType); // Send it to the client
    file.close();                                       // Close the file again
    Serial.println(String("\tSent file: ") + path + " size: " + sent);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path); // If the file doesn't exist, return false
  return false;
}

// multipart from Form
void handleFileUpload()
{ // upload a new file to the LittleFS
  triggerActivity();
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    String filename = upload.filename;
    if (!filename.startsWith("/"))
      filename = "/" + filename;
    Serial.print("handleFileUpload Name: ");
    Serial.println(filename);
    lastDownloadFilename = filename;
    fsUploadFile = LittleFS.open(filename, "w"); // Open the file for writing in SPIFFS (create if it doesn't exist)
    filename = String();
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (fsUploadFile)
    {                       // If the file was successfully created
      fsUploadFile.close(); // Close the file again
      Serial.print("handleFileUpload Size: ");
      Serial.println(upload.totalSize);
      server.sendHeader("Location", "/success"); // Redirect the client to the success page
      server.send(303);
    }
    else
    {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

// content in body
void handleFileStore()
{ // upload a new file to the LittleFS
  Serial.println("handleFileStore(): ");
  /*
    String message = "";
    message += "Args Count: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    Serial.println(message);
  */

  // arg - get request argument value, use arg("plain") to get POST body
  /*
  String c = server.arg("plain");
  Serial.println("body: " + c);
  */

  if (!server.hasArg("name"))
  {
    Serial.println("handleFileUpload argument 'name' not found!!");
    server.send(500, "text/plain", "500: filename not defined ");
    return;
  }

  String filename = server.arg("name");
  if (!filename.startsWith("/"))
    filename = "/" + filename;
  Serial.print("handleFileUpload Name: ");
  Serial.println(filename);
  fsUploadFile = LittleFS.open(filename, "w"); // Open the file for writing in SPIFFS (create if it doesn't exist)
  filename = String();
  if (fsUploadFile)
  {
    fsUploadFile.print(server.arg("plain")); // Write the received body to the file
    fsUploadFile.close();                    // Close the file again
    server.send(200, "text/plain", "handleFileStore() " + filename + " successful");
  }
  else
  {
    server.send(500, "text/plain", "500: couldn't create file " + filename);
  }
}