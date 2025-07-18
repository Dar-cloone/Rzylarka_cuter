#include "i2c_bridge.h"
#include <Wire.h>
#include "../machine_fsm/machine_fsm.h"
#include "../process_params/process_params.h"

/* --------- zmienne ISR --------- */
static volatile uint8_t  gCmd    = 0;
static volatile float    gParam  = 0.0f;
static volatile bool     gPend   = false;
static volatile uint8_t  gStatus = S_IDLE;

/* --------- stare komendy 0x01–0x04 --------- */
static void hHome(float){ fsmHome(); }
static void hGoto(float p){ fsmGoto(p); }
static void hCut(float){ fsmCut(0); }
static void hGlue(float p){ fsmGlue(p); }

struct Handler { uint8_t code; void(*fn)(float); };
static const Handler tbl[] = {
    {0x01, hHome},
    {0x02, hGoto},
    {0x03, hCut},
    {0x04, hGlue}
};
constexpr uint8_t N = sizeof(tbl)/sizeof(tbl[0]);

/* --------- I²C ISR --------- */
static void onReceive(int n)
{
    if(n<1) return;
    gCmd = Wire.read();

    if(n >= 5){                               // polecenie z float-payload
        union{uint8_t b[4]; float f;} u;
        for(uint8_t i=0;i<4;i++) u.b[i]=Wire.read();
        gParam = u.f;
    } else {
        gParam = 0.0f;                        // SET/GET/ew. sygnały (nieobsługiwane już)
    }
    gPend   = true;
    gStatus = S_BUSY;                         // natychmiastowe ACK
}

static void onRequest(){
    Wire.write(gStatus);
    if (gStatus == S_DONE) gStatus = S_IDLE;  // Po odczycie DONE przez mastera wróć na IDLE
}

/* --------- INIT --------- */
void bridgeInit(uint8_t addr)
{
    Wire.begin(addr);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
}

/* --------- DISPATCH w loop() --------- */
static void dispatch()
{
    if(!gPend) return;

    /* --- SET_PARAM (0x50) --- */
    if(gCmd==0x50){
        uint8_t id = Wire.read();
        if(id>=1 && id<=5){
            if(id>=4){                    /* uint16 */
                uint16_t v = Wire.read() | (Wire.read()<<8);
                if(id==4) pp.pause_ms   = v;
                if(id==5) pp.timeout_ms = v;
            }else{                        /* float */
                union{uint8_t b[4]; float f;} u;
                for(int i=0;i<4;i++) u.b[i]=Wire.read();
                if(id==1) pp.feed_mm    = u.f;
                if(id==2) pp.glue_ul    = u.f;
                if(id==3) pp.retract_ul = u.f;
            }
            ppSave(id);                  // do EEPROM
            gStatus = S_ACK_OK;
        }else gStatus = S_ERR;
        gPend=false; return;
    }

    /* --- GET_PARAM (0x51) --- */
    if(gCmd==0x51){
        uint8_t id = Wire.read();
        switch(id){
            case 1: Wire.write((uint8_t*)&pp.feed_mm,4);    break;
            case 2: Wire.write((uint8_t*)&pp.glue_ul,4);    break;
            case 3: Wire.write((uint8_t*)&pp.retract_ul,4); break;
            case 4: Wire.write((uint8_t*)&pp.pause_ms,2);   break;
            case 5: Wire.write((uint8_t*)&pp.timeout_ms,2); break;
            default: Wire.write(0x55);                      break;
        }
        gPend=false; return;
    }

    /* --- stare 0x01–0x04 --- */
    for(uint8_t i=0;i<N;i++){
        if(tbl[i].code == gCmd){ tbl[i].fn(gParam); gPend=false; return; }
    }

    gStatus = S_ERR;   // nieznany kod
    gPend   = false;
}

void bridgeUpdate()
{
    dispatch();

    if     (fsmError()) gStatus = S_ERR;
    else if(fsmBusy())  gStatus = S_BUSY;
    else if (gStatus == S_BUSY) gStatus = S_DONE;  // zakończenie zadania → S_DONE
    // S_DONE pozostaje aż master odczyta i przełączy na IDLE
}

uint8_t bridgeStatus(){ return gStatus; }
