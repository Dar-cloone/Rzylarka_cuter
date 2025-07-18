#include "cutter.h"
#include "cutter_config.h"

/* ───────── stan wewnętrzny ───────── */
enum class State : uint8_t { IDLE, OPEN, SETTLE };
static State     gState      = State::IDLE;
static uint32_t  gTimestamp  = 0;          // znacznik startu fazy

/* ───────── pomocnicze ─────────────── */
static inline void valveOpen()  { digitalWrite(CUT_VALVE_PIN, HIGH); } // HIGH = open
static inline void valveClose() { digitalWrite(CUT_VALVE_PIN, LOW ); } // LOW  = closed

/* ───────── API ────────────────────── */
void cutterInit()
{
    pinMode(CUT_VALVE_PIN, OUTPUT);
    valveClose();                         // zawsze startuje zamknięty
    gState = State::IDLE;
}

bool cutterCut()
{
    if (gState != State::IDLE) return false;   // już trwa

    valveOpen();
    gTimestamp = millis();
    gState = State::OPEN;
    return true;
}

bool cutterBusy() { return gState != State::IDLE; }

void cutterUpdate(uint32_t now)
{
    switch (gState)
    {
        case State::OPEN:
            if (now - gTimestamp >= CUT_OPEN_MS)
            {
                valveClose();
                gTimestamp = now;
                gState = State::SETTLE;
            }
            break;

        case State::SETTLE:
            if (now - gTimestamp >= CUT_SETTLE_MS)
            {
                gState = State::IDLE;         // cykl zakończony
            }
            break;

        default: break;  // IDLE – nic do zrobienia
    }
}
