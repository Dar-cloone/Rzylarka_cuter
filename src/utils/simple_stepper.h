// ==============================================================
//  simple_stepper.h  – lekka klasa sterownika krokowego (header-only)
//  Zero blokad: run() wywołuj w pętli; delay między krokami = _stepDelay µs
// ==============================================================
#ifndef SIMPLE_STEPPER_H
#define SIMPLE_STEPPER_H

#include <Arduino.h>

class SimpleStepper {
public:
  /* ctor: podaj piny STEP i DIR drivera */
  SimpleStepper(uint8_t stepPin, uint8_t dirPin)
    : _stepPin(stepPin), _dirPin(dirPin) {}

  /* konfiguracja portów – wywołaj raz w setup() */
  void begin() {
    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin,  OUTPUT);
  }

  /* ustaw odstęp między impulsami STEP [µs] */
  void setStepDelayUs(unsigned int d) { _stepDelay = d; }

  /* dodaj ruch o liczbę kroków (+ ⇒ DIR=HIGH, – ⇒ DIR=LOW) */
  void move(long steps) {
    _targetPos += steps;
    digitalWrite(_dirPin, steps >= 0 ? HIGH : LOW);
  }

  /* wywołuj często – generuje impulsy dopóki nie osiągnie _targetPos */
  void run() {
    if (!isRunning()) return;
    unsigned long now = micros();
    if (now - _lastStepTime >= _stepDelay) {
      _lastStepTime = now;
      digitalWrite(_stepPin, HIGH);
      digitalWrite(_stepPin, LOW);
      _currentPos += (_targetPos > _currentPos) ? 1 : -1;
    }
  }

  /* true ⇔ w trakcie ruchu */
  bool isRunning() const { return _currentPos != _targetPos; }

  /* numer bieżącego kroku (możesz przełożyć sam na mm) */
  long currentPosition() const { return _currentPos; }

  /* ręczne nadanie pozycji bez ruchu */
  void setCurrentPosition(long p) { _currentPos = _targetPos = p; }

private:
  uint8_t _stepPin, _dirPin;
  long    _currentPos = 0, _targetPos = 0;
  unsigned int _stepDelay     = 500;   // domyślnie ≈2 kHz
  unsigned long _lastStepTime = 0;
};

#endif  // SIMPLE_STEPPER_H
