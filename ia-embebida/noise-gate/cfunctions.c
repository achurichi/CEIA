#include <stdio.h>
#include <stdbool.h>

#define VALUE_MAX 32767
#define VALUE_MIN -32768

typedef struct
{
  float open;
  float close;
  float hold;
  int samplerate;
} Config;

typedef struct
{
  int lastIdxOpen;
  bool isOpen;
} PrevData;

typedef struct
{
  int output;
  bool keepOpen;
} Result;

float normalize(float value)
{
  return (value - VALUE_MIN) / (VALUE_MAX - VALUE_MIN);
}

Result noiseGate(int sample, int idx, PrevData prevData, Config config)
{
  float sampleSquare = normalize((float)sample) * normalize((float)sample);
  float open = normalize(config.open) * normalize(config.open);
  float close = normalize(config.close) * normalize(config.close);
  int samplesHold = (int)(config.hold * config.samplerate);
  Result result;
  result.keepOpen = prevData.isOpen;

  if (sampleSquare >= open)
    result.keepOpen = true;
  else if (prevData.isOpen &&
           sampleSquare < close &&
           idx > prevData.lastIdxOpen + samplesHold)
    result.keepOpen = false;

  result.output = result.keepOpen ? sample : 0;

  return result;
}
