#include <Arduino.h>
#include <stdio.h>
#include <cassert>
#include "KeyState.hpp"

static const unsigned LONG_PRESS_THRESHOLD_MS = 500;
static const unsigned DOUBLE_PRESS_THRESHOLD_MS = 350;
static const unsigned DEBOUNCE_THRESHOLD_MS = 350;

KeyState::KeyPressKind
KeyState::Update(PinStatus pinStatus)
{
  KeyPressKind result = KeyPressKind::None;
  ulong elapsed = 0;

  switch (state)
  {
  case States::WaitingForHigh:
  {
    if (pinStatus != HIGH)
    {
      // Stay waiting
      break;
    }
    state = States::WaitingForLow;
    longPressTimerStart = dblPressTimerStart = millis();
    break;
  }
  case States::WaitingForLow:
  {
    if (pinStatus != LOW)
    {
      // stay waiting for a LOW.
      break;
    }
    elapsed = millis() - longPressTimerStart;
    if (elapsed > LONG_PRESS_THRESHOLD_MS)
    {
      state = States::LongPressDetected;
      break;
    }
    elapsed = millis() - dblPressTimerStart;
    if (elapsed > DOUBLE_PRESS_THRESHOLD_MS)
    {
      state = States::SinglePressDetected;
      break;
    }
    // this is the tricky case: low pin within the double-press threshold.
    state = States::WaitingForDoubleHigh;
    break;
  }
  case States::WaitingForDoubleHigh:
  {
    elapsed = millis() - dblPressTimerStart;
    if (elapsed > DOUBLE_PRESS_THRESHOLD_MS)
    {
      if (pinStatus == LOW)
      {
        state = States::SinglePressDetected;
        break;
      }
      // it might be preferable to just eat this keypress as a "failed" double click. Doing so fires no events and so
      // is benign -- at worst, the user has to try again.
      state = States::WaitingForExpiredDoubleLow;
      break;
    }
    if (pinStatus == HIGH)
    {
      state = States::WaitingForDoubleLow;
      break;
    }
    // else, we're within the double threshold and the pin is low, so
    // keep waiting.
    break;
  }
  case States::WaitingForExpiredDoubleLow:
  {
    if (pinStatus != LOW)
    {
      // keep waiting for a low
      break;
    }
    state = States::SinglePressDetected;
    break;
  }
  case States::WaitingForDoubleLow:
  {
    if (pinStatus != LOW)
    {
      // keep waiting
      break;
    }
    state = States::DoublePressDetected;
    break;
  }
  case States::LongPressDetected:
  {
    result = KeyPressKind::LongPress;
    debounceTimerStart = millis();
    state = States::Debouncing;
    break;
  }
  case States::SinglePressDetected:
  {
    result = KeyPressKind::SinglePress;
    debounceTimerStart = millis();
    state = States::Debouncing;
    break;
  }
  case States::DoublePressDetected:
  {
    result = KeyPressKind::DoublePress;
    debounceTimerStart = millis();
    state = States::Debouncing;
    break;
  }
  case States::Debouncing:
  {
    elapsed = millis() - debounceTimerStart;
    if (elapsed > DEBOUNCE_THRESHOLD_MS)
    {
      state = States::WaitingForHigh;
    }
    break;
  }
  default:
  {
    assert(false);
  }
  }
  return result;
}
