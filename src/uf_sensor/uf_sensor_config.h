#pragma once
#include <Arduino.h>

/* ---- pin ADC ---- */
constexpr uint8_t UF_SENSOR_PIN = A0;

/* ---- progi ---- */
constexpr int UF_DETECT_LEVEL = 40;   // ≤ 50  → rurka zasłania
constexpr int UF_CLEAR_LEVEL  = 41;   // ≥ 60  → rurka odsłonięta

/* ---- tempo próbkowania ---- */
constexpr unsigned UF_SAMPLE_MS = 20; // 50 Hz
