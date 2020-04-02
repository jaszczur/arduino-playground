#include "Atm_car.hpp"

Atm_car& Atm_car::begin() {
  const static state_t state_table[] PROGMEM = {
      /*               ON_ENTER    ON_LOOP  ON_EXIT   E_STOP  E_FORWARD  E_BACKWARD   E_LEFT   E_RIGHT  ELSE */
      /* STOPPED  */       STOP,        -1,    EXIT,      -1,   FORWARD,   BACKWARD,    LEFT,    RIGHT,   -1,
      /* FORWARD  */  M_FORWARD,        -1,    EXIT, STOPPED,   FORWARD,    STOPPED,    LEFT,    RIGHT,   -1,
      /* BACKWARD */ M_BACKWARD,        -1,    EXIT, STOPPED,   STOPPED,   BACKWARD,    LEFT,    RIGHT,   -1,
      /* LEFT     */     M_LEFT,        -1,    EXIT, STOPPED,   FORWARD,   BACKWARD,    LEFT,  STOPPED,   -1,
      /* RIGHT    */    M_RIGHT,        -1,    EXIT, STOPPED,   FORWARD,   BACKWARD, STOPPED,    RIGHT,   -1,
  };
  Machine::begin(state_table, ELSE);
  motor.init();
  return *this;
}

void Atm_car::action(int actionId) {
  switch (actionId) {
  case STOP:
    motor.dcMotorStop(leftMotor);
    motor.dcMotorStop(rightMotor);
    leftMotorMult = 0;
    rightMotorMult = 0;
    return; // Do not touch engines anymore

  case M_LEFT:
    leftMotorMult = -1;
    rightMotorMult = 1;
    break;
  case M_RIGHT:
    leftMotorMult = 1;
    rightMotorMult = -1;
    break;
  case M_FORWARD:
    leftMotorMult = 1;
    rightMotorMult = 1;
    break;
  case M_BACKWARD:
    leftMotorMult = -1;
    rightMotorMult = -1;
    break;
  case EXIT:
    // is it true exit?
    if (current != next) {
      actionRepeatCount = 0;
      return; //??
    }
  default:
    return;
  }

  int speed = min(255, startingSpeed + actionRepeatCount * 25);
  motor.dcMotorRun(leftMotor, speed * leftMotorMult);
  motor.dcMotorRun(rightMotor, speed * rightMotorMult);
  actionRepeatCount++;
}

int Atm_car::event(int eventId) {
  return 0;
}

Atm_car &Atm_car::trace(Stream &stream) {
  Machine::setTrace(
      &stream, atm_serial_debug::trace,
      "Car\0E_STOP\0E_FORWARD\0E_BACKWARD\0E_LEFT\0E_RIGHT\0ELSE\0STOPPED\0FORWARD\0BACKWARD\0LEFT\0RIGHT");
  return *this;
}
