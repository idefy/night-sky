#include <Servo.h>
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <ezTime.h>
#include <time.h>

//internal libraries
#include "Config.h"
#include "debug.h"
#include "Event.h"
#include "Sky.h"

//===============================================================
// SETUP VALUES
//===============================================================

// SERVER&WIFI
//---------------------------------------------------------------
// Replace with your network credentials
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Set web server port number to 80
AsyncWebServer server(80);

// TIMEZONE
//---------------------------------------------------------------
// use official list here: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
const char* tzlocale = "America/Toronto";

// CONFIG
//---------------------------------------------------------------
// GPIOs
const byte moon_pin = 27;
const byte stars_pin = 25;
const byte shadow_pin = 21;
const byte button_pin = 4;

//internals
Config theConfig;
Sky theSky(moon_pin, stars_pin, shadow_pin);
Timezone LocalTime;
Event theEvent;
EventItem ei;

//===============================================================
// HANDLERS
//===============================================================
void handleRoot(AsyncWebServerRequest *request) {
  DEBUG_PRINTLN("Serving index.html");
  request->send(SPIFFS, "/index.html", "text/html");
}

void handleFavIcon(AsyncWebServerRequest *request) {
  DEBUG_PRINTLN("Serving favicon");
  request->send(SPIFFS, "/favicon.ico", "text/css");
}

void handleCSS(AsyncWebServerRequest *request) {
  DEBUG_PRINTLN("Serving style.css");
  request->send(SPIFFS, "/style.css", "text/css");
}

void handleJS(AsyncWebServerRequest *request) {
  DEBUG_PRINTLN("Serving javascript.js");
  request->send(SPIFFS, "/javascript.js", "text/javascript");
}

void handleConfig(AsyncWebServerRequest *request) {
  DEBUG_PRINTLN("Serving config");
  request->send(200, "text/plain", theConfig.toJson());
}

void handleReset(AsyncWebServerRequest *request) {
  theConfig.reset();
  request->send(200, "text/plain", "reseted");
}

void handleSaveConfig(AsyncWebServerRequest *request) {
  DEBUG_PRINTLN("Serving save config with: ");
  if (request->hasArg("data")) {
    String arg = request->arg("data");
    DEBUG_PRINTLN(arg);
    theConfig.fromJson(arg);
    theConfig.save();
    handleEvent();
  }
  request->send(200, "text/plain", theConfig.toJson());
}

void handleStatus(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", theSky.statusToJson());
}

void handleChangeStatus(AsyncWebServerRequest *request) {
  DEBUG_PRINTLN("Serving change status");
  int args = request->args();

  //MOON
  if (request->hasArg("moon") ) {
    DEBUG_PRINT("Moon turned on: ");
    DEBUG_PRINTLN(request->arg("moon"));
    if (request->arg("moon") == "true") {
      theSky.startMoon(theConfig.moonSleep * 60);
    } else {
      theSky.endMoon();
    }
  }

  //STARS
  if (request->hasArg("stars") ) {
    DEBUG_PRINT("Stars turned on: ");
    DEBUG_PRINTLN(request->arg("stars"));
    if (request->arg("stars") == "true") {
      theSky.startStars(theConfig.starsSleep * 60);
    } else {
      theSky.endStars();
    }
  }

   //PHASE MANUAL MODE
  if (request->hasArg("phaseManual") ) {
    DEBUG_PRINT("Phase manula mode on: ");
    DEBUG_PRINTLN(request->arg("phaseManual"));
    if (request->arg("phaseManual") == "true") {
      theSky.setPhaseManual(true);
    } else {
      theSky.setPhaseManual(false);
    }
  }

  //PHASE
  DEBUG_PRINT("phase manual on : ");
  DEBUG_PRINTLN(theSky.getPhaseManual());
  if(theSky.getPhaseManual() == true) {
    if (request->hasArg("phase") ) {
      DEBUG_PRINT("Phase changed to : ");
      DEBUG_PRINTLN(request->arg("phase"));
      theSky.setPhase(request->arg("phase").toInt());
    }
  }

  request->send(200, "text/plain", theSky.statusToJson());
}

void handleMode(AsyncWebServerRequest *request) {
  DEBUG_PRINTLN("Serving mode");
  int m = 0;
  if (request->hasArg("mode") ) {
    DEBUG_PRINT("Mode: ");
    DEBUG_PRINTLN(request->arg("mode"));
    m = request->arg("mode").toInt();
    theConfig.mode = m;
    theConfig.save();
  }
  handleEvent();
  request->send(200, "text/plain", "{mode:" + String(m) + "}");
}

//===============================================================
// EVENTS
//===============================================================
// event handlers
void eventTrigger() {
  DEBUG_PRINTLN("event fired");
  theSky.setPhaseManual(false);
  theSky.startMoon(ei.moon * 60, ei.wake);
  theSky.startStars(ei.stars * 60);
  handleEvent();
}

void handleEvent() {
  //removes existing events
  deleteEvent(eventTrigger);

  ei = theEvent.nextEvent();
  //DEBUG_PRINTLN(ei.time);
  //DEBUG_PRINTLN("Event time:" + dateTime(ei.time));

  if (ei.setable && ei.time != 0) {
    LocalTime.setEvent(eventTrigger, ei.time);
  }
}
//===============================================================
// SETUP & LOOP
//===============================================================

void setup() {

  Serial.begin(115200);
  while (!Serial) continue;

  // Initialize SPIFFS library
  while (!SPIFFS.begin()) {
    DEBUG_PRINTLN(F("Failed to initialize SPIFFS"));
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
  DEBUG_PRINTLN("UTC: " + UTC.dateTime());
  LocalTime.setLocation(tzlocale);
  DEBUG_PRINTLN("Local time: " + LocalTime.dateTime());

  //loads config
  //theConfig.reset();
  theConfig.load();
  theEvent.begin(LocalTime, theConfig);
  theSky.setConfig(theConfig);

  //server
  server.on("/", handleRoot);
  server.on("/favicon.ico", handleFavIcon);
  server.on("/style.css", handleCSS);
  server.on("/javascript.js", handleJS);
  server.on("/reset", handleReset);
  server.on("/config", handleConfig);
  server.on("/saveconfig", handleSaveConfig);
  server.on("/status", handleStatus);
  server.on("/changeStatus", handleChangeStatus);
  server.on("/mode", handleMode);
  server.begin();

  DEBUG_PRINTLN("HTTP server started");

  handleEvent();
}

void loop() {

  theSky.buttonListener(button_pin);
  theSky.updateStarsMoon();

  events();

  delay(1);
}
