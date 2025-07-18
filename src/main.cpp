#include <Arduino.h>           // watchdogEnable / watchdogReset
#include "comm/i2c_bridge.h"
#include "machine_fsm/machine_fsm.h"

void setup()
{
    fsmInit();                 // inicjalizacja wszystkich modułów
    bridgeInit(0x12);          // adres I²C tego slave’a (0x12 = 18 dec)

    /* ---------- sprzętowy WDT 2-sekundowy ---------- */
    #ifdef watchdogEnable       // dostępne w core Renesas v1.4+
        watchdogEnable(2000);   // 2000 ms
    #endif
}

void loop()
{
    bridgeUpdate();             // obsługa I²C (Rx/Tx + STATUS)
    fsmUpdate(millis());        // serwis FSM i wszystkich sterowników

    /* odśwież watchdog przy każdej iteracji */
    #ifdef watchdogReset
        watchdogReset();
    #endif
}
