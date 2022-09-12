#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
  uint16_t open;
  uint16_t close;
  uint32_t samplesHold;
  uint32_t openCounter;
  bool isOpen;
} Config;

uint16_t toUint16(int value)
{
  return value + 32768;
}

bool noiseGate(uint16_t sample, Config *config)
{
  if (sample >= config->open)
  {
    config->isOpen = true;
    config->openCounter = 0;
  }
  else if (config->isOpen)
  {
    if (sample < config->close && config->openCounter >= config->samplesHold)
      config->isOpen = false;
    else
      config->openCounter++;
  }

  return config->isOpen;
}
