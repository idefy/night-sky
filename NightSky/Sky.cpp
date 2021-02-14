/*
  Sky.cpp - nightsky GPIO interface manager.
*/

#include "Sky.h"
#include "Arduino.h"
#include "Config.h"
#include <Servo.h>
#include <time.h>
#include <ezTime.h>

Sky::Sky(byte moon, byte stars, byte servo)
{
  _moonPin = moon;
  _starsPin = stars;
  _servoPin = servo;

  pinMode(_moonPin, OUTPUT);
  pinMode(_starsPin, OUTPUT);
  //_servoObj.attach(_servoPin);
}

void Sky::setConfig(Config conf) {
  _conf = conf;
}

void Sky::_setMoon(bool isOn) {
  _moonOn = isOn;
  digitalWrite(_moonPin, _moonOn);
}

void Sky::_setStars(bool on) {
  _starsOn = on;
  digitalWrite(_starsPin, _starsOn);
}

void Sky::setPhaseManual(bool on) {
  _phaseManualOn = on;
}

void Sky::setPhase(int angle) {
  if (_phaseAngle != angle && !_moving) {
    //need to attach and detach servo everytime for bad servos
    _moving = true;
    _servoObj.attach(_servoPin);
    _phaseAngle = angle;
    _servoObj.write(_phaseAngle);
    delay(2000);
    _servoObj.detach();
    _moving = false;
  }
}

bool Sky::getMoon() {
  return _moonOn;
}

bool Sky::getStars() {
  return _starsOn;
}

int Sky::getPhase() {
  return _phaseAngle;
}

bool Sky::getPhaseManual() {
  return _phaseManualOn;
}
void Sky::startMoon(uint16_t seconds, bool wake) {
  _setMoon(true);
  _moonDur = seconds;
  if (wake) {
    _moonOpenDir = true;
    setPhase(180);
  } else {
    _moonOpenDir = false;
    setPhase(0);
  }
  _start = now();
}
void Sky::endMoon() {
  _moonDur = 0;
  _setMoon(false);
}
void Sky::startStars(uint16_t seconds) {
  _starsDur = seconds;
  _setStars(true);
  _start = now();
}
void Sky::endStars() {
  _starsDur = 0;
  _setStars(false);
}

/**
   updates the moon and stars
*/
void Sky::updateStarsMoon() {
  time_t n = now();
  int elapsed = n - _start;
  if (_moonOn) {
    if (elapsed >= _moonDur) {
      endMoon();
    } else {
      if (!_phaseManualOn) {
        int angle = round((float)180 / _moonDur * elapsed);
        if (_moonOpenDir) {
          angle = 180 - angle;
        }
        setPhase(angle);
      }
    }
  }

  if (_starsOn && elapsed >= _starsDur) {
    endStars();
  }
}
/**
   cycle is as follows with x=stars and o=moon
   x:OFF, o:OFF
   x:ON , o:OFF
   x:ON , o:ON
   x:ON, o:OFF
*/
void Sky::lightCycle() {
  DEBUG_PRINTLN("buttonCycle");
  if (_moonOn && _starsOn) {
    DEBUG_PRINTLN("x:OFF, o:ON");
    endStars();
    //setStars(false);
  } else {
    if (!_moonOn && !_starsOn) {
      DEBUG_PRINTLN("x:ON , o:OFF");
      startStars(_conf.starsSleep * 60);
      //setStars(true);
    } else {
      if (_starsOn) {
        DEBUG_PRINTLN("x:ON , o:ON");
        //setMoon(true);
        startMoon(_conf.moonSleep * 60, false);
      } else { //moon is on
        DEBUG_PRINTLN("x:OFF, o:OFF");
        //setMoon(false);
        endMoon();
      }
    }
  }
}

/**
   listen to button press and reacts :
   - on short press cycles throught the lights
   - on long press permutes between full/new moon phases
*/
void Sky::buttonListener(byte pin) {
  //button handling
  _bCurrentState = digitalRead(pin);

  if (_bLastState == LOW && _bCurrentState == HIGH)  {     // button is pressed
    _bPressedTime = millis();
  }
  else if (_bLastState == HIGH && _bCurrentState == LOW) { // button is released
    _bReleasedTime = millis();
    long pressDuration = _bReleasedTime - _bPressedTime;

    if ( pressDuration < SHORT_PRESS_TIME ) {
      DEBUG_PRINTLN("A short press is detected");
      lightCycle();
    } else {
      if ( pressDuration >= LONG_PRESS_TIME ) {
        DEBUG_PRINTLN("A long press is detected");
        if (getPhase() > 0) {
          setPhase(0);
        } else {
          setPhase(180);
        }
      }
    }
  }

  // save the the last state
  _bLastState = _bCurrentState;
}

String Sky::statusToJson() {
  DynamicJsonDocument doc(256);

  doc["moon"] = getMoon();
  doc["stars"] = getStars();
  doc["phase"] = getPhase();
  doc["phaseManual"] = getPhaseManual();
  String output;
  serializeJson(doc, output);
  return output;
}
