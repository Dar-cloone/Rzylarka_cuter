#pragma once
#include <Arduino.h>

enum : uint8_t {
    S_IDLE    = 0x00,
    S_BUSY    = 0x01,
    S_ERR     = 0xEE,
    S_ACK_OK  = 0xAA        // ACK dla SET_PARAM
};

void  bridgeInit(uint8_t i2cAddr);  // wywołaj w setup()
void  bridgeUpdate();               // wywołuj w loop()
uint8_t bridgeStatus();             // jeśli potrzebne w debug
