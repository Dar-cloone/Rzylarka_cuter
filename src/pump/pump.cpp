#include "pump.h"
#include "pump_config.h"
#include "../utils/simple_stepper.h"

/* ── sterownik ─────────────────── */
static SimpleStepper motor(PUMP_STEP_PIN, PUMP_DIR_PIN);

/* ── FSM ───────────────────────── */
enum class Phase : uint8_t {
    IDLE,
    FWD_ACC, FWD_RUN, FWD_DEC,
    RET_ACC, RET_RUN, RET_DEC
};
static Phase   gState        = Phase::IDLE;

/* ── parametry bieżącego ruchu ─── */
static long    gStepsTotal   = 0;
static int     gRampSteps    = 0;
static uint16_t usMin        = 0;
static uint16_t usMax        = 0;

/* ── driver EN zawsze LOW ──────── */
static inline void driverEnable() {
    pinMode(PUMP_EN_PIN, OUTPUT);
    digitalWrite(PUMP_EN_PIN, LOW);
}

/* ── start sekwencji FWD → RET ─── */
bool pumpDroplet(double ul)
{
    if (gState != Phase::IDLE) return false;

    long fwdSteps = lround(ul          * PUMP_STEPS_PER_UL);
    long retSteps = lround(RETRACT_UL * PUMP_STEPS_PER_UL);
    if (fwdSteps <= 0 || retSteps <= 0) return false;   // zabezp.

    driverEnable();

    /* --- FWD ------------- */
    gStepsTotal = fwdSteps;
    gRampSteps  = (gStepsTotal < 2*RAMP_STEPS_FWD) ? gStepsTotal/2 : RAMP_STEPS_FWD;
    usMin       = PULSE_MIN_US_FWD;
    usMax       = PULSE_MAX_US_FWD;

    motor.setCurrentPosition(0);
    motor.setStepDelayUs(usMax);
    motor.move(+gStepsTotal);          // kierunek pompowania
    gState = gRampSteps ? Phase::FWD_ACC : Phase::FWD_RUN;
    return true;
}

/* ── API status ────────── */
bool pumpBusy() { return gState != Phase::IDLE; }

/* ── INIT ──────────────── */
void pumpInit()
{
    driverEnable();
    motor.begin();
}

/* ── pomoc: aktualizacja rampy ─── */
static void updateRamp(long done, long total)
{
    /* decydujemy na podstawie gState, usMin/usMax, gRampSteps */
    if (gRampSteps == 0) return;

    if (gState == Phase::FWD_ACC || gState == Phase::RET_ACC) {
        unsigned delta = (usMax - usMin) / gRampSteps;
        motor.setStepDelayUs(usMax - delta * done);
    }
    else if (gState == Phase::RET_DEC || gState == Phase::FWD_DEC) {
        long d = done - (total - gRampSteps);
        unsigned delta = (usMax - usMin) / gRampSteps;
        motor.setStepDelayUs(usMin + delta * d);
    }
}

/* ── UPDATE ────────────── */
void pumpUpdate(uint32_t /*now_us*/)
{
    if (gState == Phase::IDLE) return;

    motor.run();
    long done = labs(motor.currentPosition());

    /* ------ sekcja FWD ------ */
    if (gState == Phase::FWD_ACC) {
        updateRamp(done, gStepsTotal);
        if (done >= gRampSteps) gState = Phase::FWD_RUN;
    }
    else if (gState == Phase::FWD_RUN) {
        if (done >= gStepsTotal - gRampSteps) gState = Phase::FWD_DEC;
    }
    else if (gState == Phase::FWD_DEC) {
        updateRamp(done, gStepsTotal);
    }

    /* ------ FWD koniec → start RET ------ */
    if ((gState == Phase::FWD_RUN || gState == Phase::FWD_DEC) && !motor.isRunning())
    {
        /* ustaw parametry retrakcji */
        gStepsTotal = lround(RETRACT_UL * PUMP_STEPS_PER_UL);
        gRampSteps  = (gStepsTotal < 2*RAMP_STEPS_RET) ? gStepsTotal/2 : RAMP_STEPS_RET;
        usMin       = PULSE_MIN_US_RET;
        usMax       = PULSE_MAX_US_RET;

        motor.setCurrentPosition(0);
        motor.setStepDelayUs(usMax);
        motor.move(-gStepsTotal);                      // kierunek retrakcji
        gState = gRampSteps ? Phase::RET_ACC : Phase::RET_RUN;
        return;
    }

    /* ------ sekcja RET ------ */
    if (gState == Phase::RET_ACC) {
        updateRamp(done, gStepsTotal);
        if (done >= gRampSteps) gState = Phase::RET_RUN;
    }
    else if (gState == Phase::RET_RUN) {
        if (done >= gStepsTotal - gRampSteps) gState = Phase::RET_DEC;
    }
    else if (gState == Phase::RET_DEC) {
        updateRamp(done, gStepsTotal);
    }

    /* ------ RET koniec → IDLE ------ */
    if ((gState == Phase::RET_RUN || gState == Phase::RET_DEC) && !motor.isRunning())
        gState = Phase::IDLE;
}
