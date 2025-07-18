#pragma once
#include <Arduino.h>

/* ── piny sterownika pompki ───────────────────────── */
constexpr uint8_t PUMP_STEP_PIN = 4;
constexpr uint8_t PUMP_DIR_PIN  = 7;
constexpr uint8_t PUMP_EN_PIN   = 8;     // LOW = driver włączony

/* ── rampa: pompowanie (kropla) ───────────────────── */
constexpr int      RAMP_STEPS_FWD   = 20;
constexpr uint16_t PULSE_MIN_US_FWD = 7000;
constexpr uint16_t PULSE_MAX_US_FWD = 25000;

/* ── rampa: retrakcja ─────────────────────────────── */
constexpr int      RAMP_STEPS_RET   = 20;
constexpr uint16_t PULSE_MIN_US_RET = 7000;
constexpr uint16_t PULSE_MAX_US_RET = 15000;

/* ── kalibracja mechaniczna ───────────────────────── */
constexpr double   PUMP_STEPS_PER_UL = 5.0;   // zmień po kalibracji

/* ── stała retrakcja (µl) po każdej kropli ────────── */
constexpr double   RETRACT_UL = 10.0;
