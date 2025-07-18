#pragma once
#include <Arduino.h>

/* podstawowe API */
void fsmInit();
void fsmUpdate(uint32_t nowMs);
bool fsmBusy();
bool fsmError();

/* komendy wywoływane z bridge’a (stare + nowe) */
void fsmHome();                        // 0x01
void fsmGoto(float mm);                // 0x02
void fsmCut(float dummy);              // 0x03
void fsmGlue(float ul);                // 0x04

void fsmStartUp();                     // alias → fsmHome()
void fsmFeedAndWait(int16_t mm10);     // wewn. (0x02)
void fsmGripReady();                   // 0x12
void fsmGripOk();                      // 0x11
