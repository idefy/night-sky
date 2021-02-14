# night-sky

WIP. This is the required ESP32 code for Night-sky project.

Link to the full project tutorial: TBD

## What is Night-sky ?
Imagine a starry night with the moon slowly phasing out to put you to sleep.
This is night sky.

## Features
- manualy turn on or off the stars or moon
- manual change the moon phase
- create a weekly schedule to wake up and fall sleep to the night sky
- has 2 schedules: **active** for when your work and **relax** for vacation days
- you can disable the schedule if you want to
- is managed over wifi with a simple HTML interface

# Setup

You will need arduino IDE or your prefered MCU IDE and an ESP32.

## Libraries
From the library manager search and install the following libraries:
- ezTime : https://github.com/ropg/ezTime
- ESP32Servo: https://madhephaestus.github.io/ESP32Servo/annota...

For these libraries, at the time of writting, you will need to download them:

- ESPAsyncWebServer: https://github.com/me-no-dev/ESPAsyncWebServer
- AsyncTCP : https://github.com/me-no-dev/AsyncTCP

Then manualy instal them by:

1. Unzip both .zip folders into the Arduino IDE installation libraries folder
2. Rename folder from ESPAsyncWebServer-master to ESPAsyncWebServer
3. Rename folder from AsyncTCP-master to AsyncTCP
4 .If your IDE is running, restart it

## Configuration

Open NightSky.ino and adjust settings as needed.

### WiFi
Change YOUR_SSID and YOUR_PASSWORD to your settings
```cpp
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
```
I strongly recommend usint static IP to map the MAC Address of your MCU, it's easier to call.
Anyways the monitors always displays the IP address if you need it.

### Timezone
[List of timezones values that can be used](https://en.wikipedia.org/wiki/List_of_tz_database_time_zones)
```cpp
const char* tzlocale = "America/Toronto";
```

### GPIO
- **moon_pin** enter the moon light pin
- **stars_pin** enter the stars light pin
- **shadow_pin** enter servo pin used to cast the shadow on the moon
- **button_pin** enter pin used for the push button (optional)

```cpp
const byte moon_pin = 27;
const byte stars_pin = 25;
const byte shadow_pin = 21;
const byte button_pin = 4;
```

## Running
1. Use the **tool>ESP32 Sketch data upload** to upload the needed data (HTML, js, css, etc...)
2. Compile and upload the code
3. If you haven't mapped the ESP32 to a static IP, then use the monitor to see the IP address
4. Open the IP address in your browser

## Reset config
You can reset the schedule to the default schedule config by calling :
```
http://YOUR_IP/reset
```
It will reset to the configuration found in data/default.jsn. Your can also modify this default config as needed and upload it to your MCU.

## Debuging

Should you need to debug, you can go in debug.h and uncomment the first line:
```cpp
#define DEBUG
```

# TODO
- document the code
- validation
- protection
- Hubitat compatibility
- translation
- additional settings ?
- real moon phase with 360 servo ?