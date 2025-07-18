#include "machine_fsm.h"
#include "feeder/feeder.h"
#include "cutter/cutter.h"
#include "pump/pump.h"
#include "uf_sensor/uf_sensor.h"
#include "pump/pump_config.h"

/* --------- TOP-LEVEL OPERACJE --------- */
enum class Op : uint8_t { IDLE, STARTUP, FEED_WAIT_CUT, ERROR };
static Op op = Op::IDLE;

/* --------- MIKRO-FAZY --------- */
enum class Ph : uint8_t {
    START,
    /* STARTUP */
    HOME, FEED20, CUT,
    /* FEED_WAIT_CUT */
    WAIT_POS, FEED_MM, WAIT_GRIP, CUT2, GLUE,
};
static Ph ph = Ph::START;

/* --------- PARAMETRY --------- */
static int16_t reqMm10 = 0;          // długość ×0,1 mm

/* --------- TIMEOUT --------- */
static uint32_t tStart = 0;
constexpr uint32_t TO_MS = 8000;
static inline void kick(uint32_t now){ tStart = now; }
static inline bool isTO(uint32_t now){ return now - tStart > TO_MS; }

/* --------- GLOBAL STOP --------- */
static bool stopped = false;
static void emergencyStop()
{
    if (stopped) return;

    /* pompka – odłącz driver */
    pinMode(PUMP_EN_PIN, OUTPUT);
    digitalWrite(PUMP_EN_PIN, HIGH);     // EN = HIGH → driver OFF

    /* jeśli masz osobne EN dla feederów, odkomentuj: */
    // pinMode(FEED_EN_PIN, OUTPUT);
    // digitalWrite(FEED_EN_PIN, HIGH);

    feederMove(0);                       // czyści kolejkę kroków
    // cutterAbort();                    // (jeśli masz funkcję awaryjnego stopu)

    stopped = true;
}

/* --------- SERWIS MODUŁÓW --------- */
static void driversSvc(uint32_t nowMs){
    ufSensorUpdate(nowMs);
    feederUpdate(nowMs);
    cutterUpdate(nowMs);
    pumpUpdate(micros());

    if (op != Op::IDLE && isTO(nowMs)) op = Op::ERROR;
    if (op == Op::ERROR) emergencyStop();
}

/* --------- INIT --------- */
void fsmInit(){
    ufSensorInit();
    feederInit();
    cutterInit();
    pumpInit();
}

/* --------- STATUS --------- */
bool fsmBusy()  { return op != Op::IDLE && op != Op::ERROR; }
bool fsmError() { return op == Op::ERROR; }

/* --------- WEJŚCIA ZEWNĘTRZNE --------- */
static void begin(Op o){ if(!fsmBusy()){ op = o; ph = Ph::START; stopped = false; } }

void fsmStartUp()                   { begin(Op::STARTUP); }
void fsmFeedAndWait(int16_t mm10)   { reqMm10 = mm10; begin(Op::FEED_WAIT_CUT); }
void fsmGripReady()                 { if(op == Op::FEED_WAIT_CUT && ph == Ph::WAIT_POS)  ph = Ph::FEED_MM;  }
void fsmGripOk()                    { if(op == Op::FEED_WAIT_CUT && ph == Ph::WAIT_GRIP) ph = Ph::CUT2;    }

/* --------- UPDATE --------- */
void fsmUpdate(uint32_t nowMs)
{
    driversSvc(nowMs);
    if(op == Op::IDLE || op == Op::ERROR) return;

    switch(op)
    {
    /* === ROZRUCH === */
    case Op::STARTUP:
        switch(ph){
        case Ph::START:  feederHome();          ph = Ph::HOME;   kick(nowMs); break;
        case Ph::HOME:   if(!feederBusy()){ feederMove(20); ph = Ph::FEED20; kick(nowMs);} break;
        case Ph::FEED20: if(!feederBusy()){ cutterCut();    ph = Ph::CUT;    kick(nowMs);} break;
        case Ph::CUT:    if(!cutterBusy()){ op = Op::IDLE; } break;
        default: break;
        }
        break;

    /* === PRODUKCJA === */
    case Op::FEED_WAIT_CUT:
        switch(ph){
        case Ph::START:       ph = Ph::WAIT_POS; kick(nowMs); break;
        case Ph::WAIT_POS:    /* czeka na GripReady */ break;

        case Ph::FEED_MM:
            feederMove(reqMm10 / 10.0f);
            ph = Ph::WAIT_GRIP;
            kick(nowMs);
            break;

        case Ph::WAIT_GRIP:   /* czeka na GripOk */ break;

        case Ph::CUT2:
            cutterCut();
            ph = Ph::CUT;
            kick(nowMs);
            break;

        case Ph::CUT:
            if(!cutterBusy()){
                pumpDroplet(10.0);         // domyślna dawka kleju
                ph = Ph::GLUE;
                kick(nowMs);
            }
            break;

        case Ph::GLUE:
            if(!pumpBusy()) { op = Op::IDLE; }
            break;

        default: break;
        }
        break;

    default: break;
    }
}

/* --------- ADAPTERY KOMPATYBILNOŚCI 0x01…0x04 --------- */
void fsmHome()               { fsmStartUp(); }
void fsmGoto(float mm)       { if(!fsmBusy()) fsmFeedAndWait(int16_t(lround(mm*10))); }
void fsmCut(float)           { if(!fsmBusy()) { cutterCut(); op = Op::IDLE; } }
void fsmGlue(float ul)       { if(!fsmBusy()) pumpDroplet(ul); }
