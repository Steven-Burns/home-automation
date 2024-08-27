#pragma once

#include <cstdlib>
#include "GroveLightSensor12.hpp"
#include "KeyState.hpp"



// something is discouraging me from using polymorphism here, from some vague ick about virtual functions
// and how that does not seem "embedded kosher."  This is probably a silly notion that I will get over 
// soon.

struct SensorEvent 
{
    // TODO this is a misdesign: the tag should really be a static property of the event type, not
    // per-instance state, as it does not change per instance.
    static const size_t TAG_BYTES = 7;
    char tag[TAG_BYTES + 1];

    public:

    SensorEvent(const char* tag)
    {
        strncpy(this->tag, tag, TAG_BYTES);
    }

    virtual const char* ToString() const = 0;
};



struct TemperatureEvent : public SensorEvent
{
  float temperature = 0.0;
  float humidity = 0.0;

  public:

  TemperatureEvent(float t, float h) : SensorEvent("TEMP")
  {
    temperature = t;
    humidity = h;
  }

  virtual const char* ToString() const
  {
    static const size_t bufChars = 32;
    static char toStringBuffer[bufChars];

    sprintf(toStringBuffer, "%s t=%2.1fF h=%2.1f%%", tag, temperature, humidity);

    // size_t dsize = bufChars;
    // char *p = (char*) memccpy(toStringBuffer, tag, '\0', dsize);
    // dsize -= (p - toStringBuffer - 1);
    // p = (char*) memccpy(p - 1, ftoa(temperature), '\0', dsize);
    // dsize -= (p - toStringBuffer - 1);
    // memccpy(p - 1, ftoa(humidity), '\0', dsize);

    return toStringBuffer;
  }
};



struct MotionEvent : public SensorEvent
{
  bool isMovementDetected = false;

  MotionEvent(bool isMovementDetected) : SensorEvent("MOTN")
  {
    this->isMovementDetected = isMovementDetected;
  }

  virtual const char* ToString() const 
  {
    static const size_t bufChars = 32;
    static char toStringBuffer[bufChars];

    sprintf(toStringBuffer, "%s detected=%s", tag, isMovementDetected ? "yes" : "no");
    return toStringBuffer;
  }
};



struct KeyPressEvent : public SensorEvent
{
  KeyState::KeyPressKind kind = KeyState::KeyPressKind::None;
  uint keyNumber = 0;

  KeyPressEvent(uint keyNbr, KeyState::KeyPressKind k) : SensorEvent("KEYPR")
  {
    keyNumber = keyNbr;
    kind = k;
  }

  virtual const char* ToString() const 
  {
    static const size_t bufChars = 32;
    static char toStringBuffer[bufChars];

    sprintf(toStringBuffer, "%s k=%d press=%s", tag, keyNumber, KeyState::KeyPressKindToString(kind));
    return toStringBuffer;
  }
};



struct LightLevelEvent : public SensorEvent
{
  GroveLightSensor::LightLevel lightLevel = GroveLightSensor::LightLevel::Dark;

  LightLevelEvent(GroveLightSensor::LightLevel level) : SensorEvent("LITE")
  {
    lightLevel = level;
  }

  virtual const char* ToString() const
  {
    static const size_t bufChars = 32;
    static char toStringBuffer[bufChars];

    sprintf(toStringBuffer, "%s %s", tag, GroveLightSensor::LightLevelToString(lightLevel));
    return toStringBuffer;  
  }
};
