#include "Atm_car.hpp"
#include "Atm_ir.hpp"
#include <Arduino.h>
#include <Ultrasonic.h>

#define PIN_IRDA 8
#define PIN_SONIC_TRIG 4
#define PIN_SONIC_ECHO 5

#define MOTOR_LEFT MOTOR_CHB
#define MOTOR_RIGHT MOTOR_CHA
#define MIN_DIST_CM 20

Atm_car car(MOTOR_LEFT, MOTOR_RIGHT);
Atm_ir ir;
Ultrasonic proximity(PIN_SONIC_TRIG, PIN_SONIC_ECHO);
uint32_t lastProximityTime = 0;

void updateDistances() {
  if (millis() - lastProximityTime >= 10 && car.state() == Atm_car::FORWARD) {
    car.distance(proximity.read());
    lastProximityTime = millis();
  }
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  car.begin().trace(Serial);
  ir.begin(PIN_IRDA).trace(Serial);

  ir.onData([](int id, int code, int codeInit) {
    // Serial.print((uint16_t)code, HEX);
    // Serial.println((uint16_t)codeInit, HEX);
    if ((uint16_t)codeInit == 0xFD02) {
      switch ((uint16_t)code) {
      case 0xBD42:
        car.trigger(Atm_car::E_LEFT);
        break;
      case 0xFD02:
        car.trigger(Atm_car::E_RIGHT);
        break;
      case 0x6798:
        car.trigger(Atm_car::E_FORWARD);
        break;
      case 0x47B8:
        car.trigger(Atm_car::E_BACKWARD);
        break;
      default:
        car.trigger(Atm_car::E_STOP);
        break;
      }
    }
  });
}

void loop() {
  automaton.run();
  updateDistances();
}
