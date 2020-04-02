#pragma once
#include <Automaton.h>
#include <Grove_Motor_Driver_TB6612FNG.h>

class Atm_car : public Machine {

public:
  Atm_car(motor_channel_type_t leftMotor, motor_channel_type_t rightMotor)
    : Machine()
    , leftMotor(leftMotor)
    , rightMotor(rightMotor) {};

  /**
   * States
   */
  enum {STOPPED, FORWARD, BACKWARD, LEFT, RIGHT};
  /**
   * Events
   */
  enum {E_STOP, E_FORWARD, E_BACKWARD, E_LEFT, E_RIGHT, ELSE};
  /**
   * Actions
   */
  enum {STOP, M_FORWARD, M_BACKWARD, M_LEFT, M_RIGHT, EXIT};

  Atm_car& begin();
  Atm_car& trace(Stream &stream);
  void distance(unsigned short front);

private:
  int event(int eventId);
  void action(int actionId);
  MotorDriver motor;
  const motor_channel_type_t leftMotor;
  const motor_channel_type_t rightMotor;
  const int startingSpeed = 150;
  int leftMotorMult = 0;
  int rightMotorMult = 0;
  int actionRepeatCount = 0;
};
