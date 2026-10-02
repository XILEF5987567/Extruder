#include <Arduino.h>
#include <Bonezegei_LCD1602_I2C.h>
#include <util/atomic.h>
#include <stdio.h>

#if !defined(ARDUINO_AVR_UNO) || !defined(__AVR_ATmega328P__)
#error "Dieser Sketch ist fuer den klassischen Arduino UNO mit ATmega328P vorgesehen."
#endif

constexpr uint8_t STOP_PIN = 2;       // Oeffnerkontakt nach GND
constexpr uint8_t HEATER_PIN = 7;     // Annahme: HIGH = heizen
constexpr uint8_t DIR_PIN = 8;
constexpr uint8_t STEP_PIN = 9;
constexpr uint8_t ENABLE_PIN = 10;    // Annahme: LOW = Treiber aktiv
constexpr uint8_t THERMISTOR_PIN = A0;
constexpr uint16_t RPM = 5;
constexpr uint16_t STEPS_PER_REV = 800;
static_assert(RPM > 0 && STEPS_PER_REV > 0, "Schrittrate muss positiv sein.");
constexpr uint32_t HALF_PERIOD_US =
    60000000UL / (2UL * STEPS_PER_REV * RPM); // 7500 us
static_assert(HALF_PERIOD_US > 0, "Schrittrate ist zu hoch.");
constexpr uint32_t TEMPERATURE_INTERVAL_MS = 250UL;

Bonezegei_LCD1602_I2C lcd(0x27);
volatile bool stopLatched = false;  // Wird im laufenden Programm niemals geloescht.
bool stepHigh = false;
uint32_t lastStepUs = 0;
uint32_t lastTemperatureMs = 0;

// Keine Anzeige, Kommunikation oder Wartefunktion in der ISR.
void emergencyStopISR() {
  stopLatched = true;
  digitalWrite(HEATER_PIN, LOW);
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(ENABLE_PIN, HIGH);
}

// Nur innerhalb eines atomaren Abschnitts aus dem Vordergrund aufrufen.
void latchAndStop() {
  stopLatched = true;
  digitalWrite(HEATER_PIN, LOW);
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(ENABLE_PIN, HIGH);
}

bool runningAllowed() {
  bool allowed = false;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (stopLatched || digitalRead(STOP_PIN) == HIGH) {
      latchAndStop();
    } else {
      allowed = true;
    }
  }
  return allowed;
}

// Pruefen UND Schreiben bleiben zusammen: kein Wiedereinschalten nach ISR.
// Auch STEP-LOW und Heizung-LOW nutzen den gleichen, einfachen Zugang.
bool guardedWrite(uint8_t pin, uint8_t level) {
  bool written = false;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (stopLatched || digitalRead(STOP_PIN) == HIGH) {
      latchAndStop();
    } else {
      digitalWrite(pin, level);
      written = true;
    }
  }
  return written;
}

void updateMotor() {
  if (!runningAllowed()) return;
  const uint32_t nowUs = micros();
  // Unsigned-Differenz funktioniert auch beim Zeitgeberueberlauf.
  if (uint32_t(nowUs - lastStepUs) < HALF_PERIOD_US) return;
  const bool nextHigh = !stepHigh;
  if (!guardedWrite(STEP_PIN, nextHigh ? HIGH : LOW)) return;
  stepHigh = nextHigh;
  lastStepUs = nowUs; // Kein Nachholen verspaeteter Flanken in einem Burst.
}

void updateTemperatureAndDisplay() {
  if (!runningAllowed()) return;
  const uint32_t nowMs = millis();
  if (uint32_t(nowMs - lastTemperatureMs) < TEMPERATURE_INTERVAL_MS) return;
  lastTemperatureMs = nowMs;

  if (!runningAllowed()) return;
  const int raw = analogRead(THERMISTOR_PIN);
  if (!runningAllowed()) return;

  // Originale Rohwert-Hysterese: innerhalb 796..806 Zustand beibehalten.
  if (raw < 796) {
    if (!guardedWrite(HEATER_PIN, HIGH)) return;
  } else if (raw > 806) {
    if (!guardedWrite(HEATER_PIN, LOW)) return;
  }

  if (!runningAllowed()) return;
  Serial.println(raw);
  if (!runningAllowed()) return;

  // Diese Bibliothek nimmt Text entgegen, keine numerischen print-Aufrufe.
  char line[17];
  snprintf(line, sizeof(line), "ADC:%4u        ", (unsigned int)raw);
  if (!runningAllowed()) return;
  lcd.setPosition(0, 0);
  if (!runningAllowed()) return;
  lcd.print(line);
  if (!runningAllowed()) return;
  snprintf(line, sizeof(line), "RPM:%3u         ", (unsigned int)RPM);
  if (!runningAllowed()) return;
  lcd.setPosition(0, 1);
  if (!runningAllowed()) return;
  lcd.print(line);
  if (!runningAllowed()) return;
}

void setup() {
  // Sichere Pegel vor OUTPUT vorladen; Resetphase davor braucht Hardware-Bias.
  digitalWrite(HEATER_PIN, LOW);
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(ENABLE_PIN, HIGH);
  digitalWrite(DIR_PIN, LOW);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  pinMode(STOP_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(STOP_PIN), emergencyStopISR, RISING);
  if (!runningAllowed()) return; // Erfasst schon beim Start offenen Kontakt.

  Serial.begin(9600);
  if (!runningAllowed()) return;
  lcd.begin();
  if (!runningAllowed()) return;
  lcd.setBacklight(1);
  if (!runningAllowed()) return;

  lastStepUs = micros();
  lastTemperatureMs = millis();
  if (!guardedWrite(ENABLE_PIN, LOW)) return;
}

void loop() {
  if (stopLatched) return; // Verriegelt bleiben, keine normalen Funktionen mehr.
  if (!runningAllowed()) return;
  updateMotor();
  if (!runningAllowed()) return;
  updateTemperatureAndDisplay();
}
