#pragma once
#include <stdint.h> //uint32_t
#include <cstddef> // size_t

struct PinSample
{
  uint32_t clock; // clock value at the time the sample was taken. The clock units are unimportant to the sample.
  int pinVal;
};

class PinSampler
{
public:
  PinSample *Current() const;
  PinSample *Previous() const;

  void TakeSample(uint32_t clock, int pinValue);
  bool IsSamplePinValChanged() const;
  const char *ToString() const;

private:
  static const size_t toStringBufChars = 20;
  char toStringBuf[toStringBufChars];
  PinSample sampleA;
  PinSample sampleB;
  PinSample *current = &sampleA;
  PinSample *previous = &sampleB;
};
