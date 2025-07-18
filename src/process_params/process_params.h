#pragma once
#include <Arduino.h>
#include <EEPROM.h>

/* ---------- struktura parametrów procesu ---------- */
struct ProcessParams {
    float    feed_mm      = 20.0f;   // domyślne podanie rurki [mm]
    float    glue_ul      = 10.0f;   // kropla             [µl]
    float    retract_ul   = 5.0f;    // cofka              [µl]
    uint16_t pause_ms     = 2000;    // pauza po cyklu     [ms]
    uint16_t timeout_ms   = 8000;    // limit fazy FSM     [ms]
};

/* ---- wersja struktury (inkrementuj przy zmianach) ---- */
constexpr uint8_t PP_VERSION = 0x02;

/* ---- adresy w EEPROM ---- */
constexpr int EE_ADDR_VERSION = 0;          // 1  B
constexpr int EE_ADDR_PARAMS  = 4;          // 20 B

extern ProcessParams pp;                    // globalnie dostępne

void ppLoad();                              // wczytaj (setup)
void ppSave(uint8_t id);                    // zapisz tylko podany parametr
