#pragma once
#include <Arduino.h>

/* --- pinout (HIGH = zawór otwarty?) --- */
constexpr uint8_t CUT_VALVE_PIN = 9;   // podaj dokładny numer
/* --- czasy w ms ----------------------- */
constexpr uint16_t CUT_OPEN_MS    = 120;   // czas „otwarte” (ms)
constexpr uint16_t CUT_SETTLE_MS  = 80;    // czas stabilizacji po zamknięciu
