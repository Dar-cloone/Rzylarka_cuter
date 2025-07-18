#include "utils/simple_stepper.h"
#include "feeder.h"
#include "feeder_config.h"
#include "../uf_sensor/uf_sensor.h"

/* ------------ FSM stany ------------ */
enum class Phase : uint8_t { IDLE, FAST, BACKOFF, APPROACH, MOVE };
static Phase gPhase = Phase::IDLE;

/* ------------ silniki --------------- */
static SimpleStepper topM(FEEDER_TOP_STEP_PIN, FEEDER_TOP_DIR_PIN);
static SimpleStepper botM(FEEDER_BOT_STEP_PIN, FEEDER_BOT_DIR_PIN);

static inline void run()            { topM.run(); botM.run(); }
static inline bool done()           { return !topM.isRunning() && !botM.isRunning(); }
static inline void queue(long st)   { topM.move(st); botM.move(-st); }
static inline void stopNow()        { topM.setCurrentPosition(topM.currentPosition());
                                      botM.setCurrentPosition(botM.currentPosition()); }

/* ------------ API ------------------- */
void feederInit()
{
    pinMode(FEEDER_ENABLE_PIN, OUTPUT);
    digitalWrite(FEEDER_ENABLE_PIN, LOW);
    topM.begin();  botM.begin();
    gPhase = Phase::IDLE;
}

void feederHome()
{
    if (gPhase != Phase::IDLE) return;

    topM.setStepDelayUs(STEP_FAST_US);
    botM.setStepDelayUs(STEP_FAST_US);
    queue(lround(HOME_DIR * STEPS_MM * SEARCH_MM));     // pierwszy blok FAST
    gPhase = Phase::FAST;
}

bool feederMove(float mm)
{
    if (gPhase != Phase::IDLE) return false;

    topM.setStepDelayUs(STEP_RUN_US);
    botM.setStepDelayUs(STEP_RUN_US);
    queue(lround(mm * STEPS_MM));
    gPhase = Phase::MOVE;
    return true;
}

bool feederBusy() { return gPhase != Phase::IDLE; }

/* ------------ pętla loop() ---------- */
void feederUpdate(uint32_t /*now*/)
{
    run();

    switch (gPhase)
    {
        /* 1. FAST – jedź szybko, aż czujnik zobaczy rurkę */
        case Phase::FAST:
            if (ufSensorDetect()) {                         /* trafienie */
                stopNow();
                topM.setStepDelayUs(STEP_FINE_US);
                botM.setStepDelayUs(STEP_FINE_US);
                queue(lround(-HOME_DIR * STEPS_MM * BACKOFF_LONG_MM));  // cofka „ciągła”
                gPhase = Phase::BACKOFF;
            } else if (done()) {                            /* brak czujnika – kolejny blok */
                queue(lround(HOME_DIR * STEPS_MM * SEARCH_MM));
            }
            break;

        /* 2. BACKOFF – cofaj, dopóki czujnik się NIE odsłoni */
        case Phase::BACKOFF:
            if (!ufSensorDetect()) {                        /* czujnik odsłonięty */
                stopNow();
                queue(lround(HOME_DIR * STEPS_MM * APPROACH_STEP_MM));   // pierwszy krok dociągu
                gPhase = Phase::APPROACH;
            }
            else if (done()) {                              /* wciąż zakryty – kolejny blok cofki */
                queue(lround(-HOME_DIR * STEPS_MM * BACKOFF_LONG_MM));
            }
            break;

        /* 3. APPROACH – powolne 0.5 mm kroki, aż znów HIT==1 */
        case Phase::APPROACH:
            if (ufSensorDetect()) {                         /* precyzyjne trafienie */
                stopNow();
                topM.setCurrentPosition(0);
                botM.setCurrentPosition(0);
                gPhase = Phase::IDLE;                       /* HOME OK */
            }
            else if (done()) {                              /* nadal odsłonięty → kolejny krok */
                queue(lround(HOME_DIR * STEPS_MM * APPROACH_STEP_MM));
            }
            break;

        /* 4. MOVE – normalny podaj / retrakcja */
        case Phase::MOVE:
            if (done()) gPhase = Phase::IDLE;
            break;

        default: break;   /* IDLE */
    }
}
