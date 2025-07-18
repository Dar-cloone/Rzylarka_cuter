#pragma once
#include <Arduino.h>

/* inicjalizacja + homing */
void feederInit();               // piny, drivery, reset stanu
void feederHome();               // rozpoczyna homing (non-blocking)

/* ruch roboczy */
bool feederMove(float mm);       // +mm → w prawo, −mm → w lewo
bool feederBusy();               // true, gdy HOMING lub MOVE w toku

/* pętla serwisowa – wywołuj co iterację loop() */
void feederUpdate(uint32_t now); // podaj millis()
