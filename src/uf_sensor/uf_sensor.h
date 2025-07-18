#pragma once
#include <Arduino.h>

void ufSensorInit();
void ufSensorUpdate(uint32_t now);
bool ufSensorDetect();   // true = rurka zasłania
int  ufSensorRaw();      // surowe ADC 0-1023
