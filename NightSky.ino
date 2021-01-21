#include <Servo.h>
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <ezTime.h>
#include <time.h>

//#include "index.h"  //Web page header file
#include "Config.h"
//===============================================================
// SETUP VALUES
//===============================================================

// SERVER&WIFI
//---------------------------------------------------------------
// Replace with your network credentials
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD;

// Set web server port number to 80
AsyncWebServer server(80);

// TIMEZONE
//---------------------------------------------------------------
// use official list here: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
const char* tzlocale = "America/Toronto";

// CONFIG
//---------------------------------------------------------------


//internals
Config theConfig;
//===============================================================
// HANDLERS
//===============================================================
void handleRoot(AsyncWebServerRequest *request) {
  Serial.println("Serving index.html");
  request->send(SPIFFS, "/index.html", "text/html");
}
void handleCSS(AsyncWebServerRequest *request) {
  Serial.println("Serving style.css");
  request->send(SPIFFS, "/style.css", "text/css");
}
void handleJS(AsyncWebServerRequest *request) {
  Serial.println("Serving javascript.js");
  request->send(SPIFFS, "/javascript.js", "text/javascript");
}
void handleConfig(AsyncWebServerRequest *request) {
  Serial.println("Serving config");
  request->send(200,"text/plain",theConfig.toJson());
}
void handleReset(AsyncWebServerRequest *request) {
  theConfig.reset();
  request->send(200, "text/plain", "reseted");
}
void handleSaveConfig(AsyncWebServerRequest *request) {
  Serial.println("Serving save config with: ");
  if(request->hasArg("data")) {
    String arg = request->arg("data");
    Serial.println(arg);
    theConfig.fromJson(arg);
    theConfig.save();
  }
  request->send(200,"text/plain",theConfig.toJson());
}

//===============================================================
// SETUP & LOOP
//===============================================================

void setup() {

  Serial.begin(115200);
  while (!Serial) continue;

  // Initialize SPIFFS library
  while (!SPIFFS.begin()) {
    Serial.println(F("Failed to initialize SPIFFS"));
    delay(1000);
  }

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //timezone
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  Timezone LocalTime;
  LocalTime.setLocation(tzlocale);
  Serial.println("Local time: " + LocalTime.dateTime());

  //loads config
  theConfig.reset();
  theConfig.load();

  //server
  server.on("/", handleRoot);
  server.on("/style.css", handleCSS);
  server.on("/javascript.js", handleJS);
  server.on("/reset", handleReset);
  server.on("/config", handleConfig);
  server.on("/saveconfig", handleSaveConfig);
  server.begin();
 
  Serial.println("HTTP server started");
}

void loop() {
  delay(1);
}
