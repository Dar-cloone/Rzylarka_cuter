#pragma once
#include <Arduino.h>

/* ==== piny driverów ==== */
constexpr uint8_t FEEDER_TOP_STEP_PIN = 2;
constexpr uint8_t FEEDER_TOP_DIR_PIN  = 5;
constexpr uint8_t FEEDER_BOT_STEP_PIN = 3;
constexpr uint8_t FEEDER_BOT_DIR_PIN  = 6;
constexpr uint8_t FEEDER_ENABLE_PIN   = 8;   // LOW = driver ON

/* ==== kinematyka ==== */
constexpr int    MICROSTEPS = 4;
constexpr double STEPS_REV  = 200.0 * MICROSTEPS;
constexpr double RADIUS_MM  = 6.52;
constexpr double STEPS_MM   = STEPS_REV / (2 * PI * RADIUS_MM);

/* ==== prędkości ==== */
constexpr unsigned STEP_FAST_US  = 2500;   // szybkie szukanie  ≈400 sps
constexpr unsigned STEP_FINE_US  = 8000;   // wolny dojazd      ≈125 sps
constexpr unsigned STEP_RUN_US   = 2500;   // ruch roboczy      ≈650 sps

/* ==== homing – closed-loop ==== */
constexpr int8_t  HOME_DIR         = +1;     // +1 = czujnik po PRAWEJ, −1 = po LEWEJ
constexpr float   SEARCH_MM        = 150.0f; // jeden szybki blok „na ślepo”
constexpr float   BACKOFF_LONG_MM  = 150.0f; // duża cofka (jadą ciągle, aż HIT==0)
constexpr float   APPROACH_STEP_MM = 0.5f;   // powolny dociąg w krokach 0 ⋯ 0.5 mm
