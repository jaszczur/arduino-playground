#include <Arduino.h>
#include <Grove_Motor_Driver_TB6612FNG.h>
#include <IRSendRev.h>
#include <Ultrasonic.h>
#include "Atm_car.hpp"

#define PIN_IRDA       8
#define PIN_SONIC_TRIG 4
#define PIN_SONIC_ECHO 5

#define BIT_LEN        0
#define BIT_START_H    1
#define BIT_START_L    2
#define BIT_DATA_H     3
#define BIT_DATA_L     4
#define BIT_DATA_LEN   5
#define BIT_DATA       6

#define MOTOR_LEFT     MOTOR_CHB
#define MOTOR_RIGHT    MOTOR_CHA
#define MIN_DIST_CM    20

unsigned char irMessage[24];
uint16_t distance = 0;
int motorEvent;
Ultrasonic ultrasonic(PIN_SONIC_TRIG, PIN_SONIC_ECHO);

Atm_car car(MOTOR_LEFT, MOTOR_RIGHT);

void decodeSignal() {
  // uint32_t irData = *((uint32_t*)(irMessage + BIT_DATA));
  // reversed endianess...
  uint32_t irData =
      (((uint32_t)irMessage[BIT_DATA + 0]) << 24)
    | (((uint32_t)irMessage[BIT_DATA + 1]) << 16)
    | (((uint32_t)irMessage[BIT_DATA + 2]) << 8)
    |   (uint32_t)irMessage[BIT_DATA + 3];
  Serial.println(irData, HEX);

  switch(irData) {
  case 0x02FD42BD:
    motorEvent = Atm_car::E_LEFT;
    break;
  case 0x02FD02FD:
    motorEvent = Atm_car::E_RIGHT;
    break;
  case 0x02FD9867:
    motorEvent = Atm_car::E_FORWARD;
    break;
  case 0x02FDB847:
    motorEvent = Atm_car::E_BACKWARD;
    break;
  default:
    motorEvent = Atm_car::E_STOP;
    break;
  }
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  IR.Init(PIN_IRDA);
  car.begin().trace(Serial);

  /*
  ir.begin().onCommand([] (int id, int code, int up) {
                         switch(code) {
                         case 0x02FD42BD:
                           car.trigger(Atm_car::E_LEFT);
                           break;
                         case 0x02FD02FD:
                           car.trigger(Atm_car::E_RIGHT);
                           break;
                         case 0x02FD9867:
                           car.trigger(Atm_car::E_FORWARD);
                           break;
                         case 0x02FDB847:
                           car.trigger(Atm_car::E_BACKWARD);
                           break;
                         default:
                           car.trigger(Atm_car::E_STOP);
                           break;
                         }
                       });
  */
}

void loop() {
  //distance = ultrasonic.read();
  if (IR.IsDta()) {
    IR.Recv(irMessage);
    decodeSignal();
    car.trigger(motorEvent);
  }
  automaton.run();
}
