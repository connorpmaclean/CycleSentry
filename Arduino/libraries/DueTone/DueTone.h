#ifndef DueTone_h
#define DueTone_h

#include "Arduino.h"

void tone(uint32_t ulPin, uint32_t frequency, int32_t duration);
void noTone(uint32_t ulPin);

#endif