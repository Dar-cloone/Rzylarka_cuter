/**
 * Cutter – publiczne API.
 * 100 % non-blocking: cutterCut() tylko zainicjuje sekwencję,
 * resztę obsługuje cutterUpdate(now).
 */
#pragma once
#include <Arduino.h>

/* inicjalizacja */
void cutterInit();                       // konfiguracja pinu, stan zamknięty

/* cykl cięcia */
bool cutterCut();                        // rozpoczyna sekwencję; false gdy już trwa
bool cutterBusy();                       // true dopóki OPEN / SETTLE w toku

/* pętla serwisowa */
void cutterUpdate(uint32_t now);         // wywołuj co loop() (podaj millis)
