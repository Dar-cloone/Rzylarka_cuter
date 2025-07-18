#include "uf_sensor.h"
#include "uf_sensor_config.h"

static int      gRaw        = 0;
static bool     gDetected   = false;
static uint32_t gLastSample = 0;

/* ---------------- inicjalizacja ---------------- */
void ufSensorInit()
{
    pinMode(UF_SENSOR_PIN, INPUT);
    gRaw        = analogRead(UF_SENSOR_PIN);
    gDetected   = (gRaw <= UF_DETECT_LEVEL);
    gLastSample = millis();

#ifdef UF_SENSOR_DEBUG
    Serial.begin(115200);
    while (!Serial) { /* czekaj, aż port się otworzy */ }
    Serial.println(F("ms\tADC\tHIT"));
#endif
}

/* ---------------- pętla serwisowa --------------- */
void ufSensorUpdate(uint32_t now)
{
    if (now - gLastSample >= UF_SAMPLE_MS)
    {
        gLastSample = now;
        gRaw = analogRead(UF_SENSOR_PIN);

        if (gDetected)
        {
            if (gRaw >= UF_CLEAR_LEVEL) gDetected = false;
        }
        else
        {
            if (gRaw <= UF_DETECT_LEVEL) gDetected = true;
        }

#ifdef UF_SENSOR_DEBUG
        Serial.print(now);
        Serial.write('\t');
        Serial.print(gRaw);
        Serial.write('\t');
        Serial.println(gDetected ? 1 : 0);
#endif
    }
}

/* ---------------- getters ----------------------- */
bool ufSensorDetect() { return gDetected; }
int  ufSensorRaw()    { return gRaw;      }
