#pragma once
#include <Arduino.h>

/* inicjalizacja pinów + włączenie drivera */
void pumpInit();

/* uruchamia kroplę + automatyczną retrakcję
 *  ul – objętość kropli (>0)
 *  zwraca false, jeżeli poprzedni cykl jeszcze trwa                  */
bool pumpDroplet(double ul);

/* true, dopóki trwa FWD lub RET */
bool pumpBusy();

/* wywołuj w każdej pętli (podaj micros()) */
void pumpUpdate(uint32_t now_us);
