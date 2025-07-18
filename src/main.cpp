#include <Wire.h>
#include <Arduino.h>

volatile bool gotReceive = false;
volatile bool gotRequest = false;
volatile int  lastLen = 0;

void onReceive(int n) {
    gotReceive = true;
    lastLen = n;
}
void onRequest() {
    gotRequest = true;
    Wire.write(0xDD); // status DONE
}

void setup() {
    Serial.begin(115200);
    Wire.begin(0x12);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    Serial.println("SLAVE: start, adres 0x12");
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    if (gotReceive) {
        Serial.print("SLAVE: ODEBRANO RECEIVE z mastera! Długość: ");
        Serial.println(lastLen);
        digitalWrite(LED_BUILTIN, HIGH);
        gotReceive = false;
    }
    if (gotRequest) {
        Serial.println("SLAVE: MASTER PYTA O STATUS (onRequest) – wysyłam 0xDD");
        digitalWrite(LED_BUILTIN, LOW);
        gotRequest = false;
    }
    delay(10);
}
