#include "process_params.h"

ProcessParams pp;   // definicja globala

/* ---------- helpers EEPROM ---------- */
static void eeWrite(int addr, const uint8_t* buf, size_t n){
    for (size_t i = 0; i < n; ++i) EEPROM.update(addr + i, buf[i]);
}
static void eeRead(int addr, uint8_t* buf, size_t n){
    for (size_t i = 0; i < n; ++i) buf[i] = EEPROM.read(addr + i);
}

/* ---------- LOAD ---------- */
void ppLoad()
{
    EEPROM.begin();                              // no-op na AVR/RA4, ale bezpieczne
    uint8_t ver = EEPROM.read(EE_ADDR_VERSION);

    if (ver != PP_VERSION) {
        eeWrite(EE_ADDR_VERSION, &PP_VERSION, 1);
        eeWrite(EE_ADDR_PARAMS, (uint8_t*)&pp, sizeof(pp));   // fabryka
    } else {
        eeRead(EE_ADDR_PARAMS, (uint8_t*)&pp, sizeof(pp));
    }
}

/* ---------- SAVE jednego pola ---------- */
void ppSave(uint8_t id)
{
    uint8_t* base = (uint8_t*)&pp;
    switch (id) {
        case 1: eeWrite(EE_ADDR_PARAMS + offsetof(ProcessParams,feed_mm),
                        base + offsetof(ProcessParams,feed_mm), 4); break;
        case 2: eeWrite(EE_ADDR_PARAMS + offsetof(ProcessParams,glue_ul),
                        base + offsetof(ProcessParams,glue_ul), 4); break;
        case 3: eeWrite(EE_ADDR_PARAMS + offsetof(ProcessParams,retract_ul),
                        base + offsetof(ProcessParams,retract_ul), 4); break;
        case 4: eeWrite(EE_ADDR_PARAMS + offsetof(ProcessParams,pause_ms),
                        base + offsetof(ProcessParams,pause_ms), 2); break;
        case 5: eeWrite(EE_ADDR_PARAMS + offsetof(ProcessParams,timeout_ms),
                        base + offsetof(ProcessParams,timeout_ms), 2); break;
    }
}
