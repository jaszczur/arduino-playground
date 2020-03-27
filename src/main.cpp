#include <Arduino.h>
#include <Grove_Motor_Driver_TB6612FNG.h>
#include <IRSendRev.h>

#define PIN_IRDA       8

#define BIT_LEN        0
#define BIT_START_H    1
#define BIT_START_L    2
#define BIT_DATA_H     3
#define BIT_DATA_L     4
#define BIT_DATA_LEN   5
#define BIT_DATA       6

#define MOTOR_LEFT     MOTOR_CHB
#define MOTOR_RIGHT    MOTOR_CHA
#define DEFAULT_SPEED  100
#define SPEED_STEP     25

#define STOPPED  0
#define LEFT     1
#define RIGHT    2
#define FORWARD  3
#define BACKWARD 4

unsigned char irMessage[24];
uint8_t currentState = STOPPED;
uint8_t speed = DEFAULT_SPEED;
MotorDriver motor;

void advanceState(uint8_t action) {
  if (   (currentState == BACKWARD && action == FORWARD)
      || (currentState == FORWARD  && action == BACKWARD)) {
    Serial.println("stop");
    currentState = STOPPED;
    speed = DEFAULT_SPEED;
  } else if (action == BACKWARD || action == FORWARD) {
    Serial.println("moar");
    currentState = action;
    speed = min(speed + SPEED_STEP, 254);
  } else if (action == LEFT || action == RIGHT) {
    speed = DEFAULT_SPEED;
    currentState = action;
  } else {
    Serial.println("WTF?");
  }
}

void performMove() {
  switch (currentState) {
  case STOPPED:
    motor.dcMotorStop(MOTOR_LEFT);
    motor.dcMotorStop(MOTOR_RIGHT);
    break;
  case LEFT:
    motor.dcMotorRun(MOTOR_LEFT,  -speed);
    motor.dcMotorRun(MOTOR_RIGHT,  speed);
    break;
  case RIGHT:
    motor.dcMotorRun(MOTOR_LEFT,   speed);
    motor.dcMotorRun(MOTOR_RIGHT, -speed);
    break;
  case FORWARD:
    motor.dcMotorRun(MOTOR_LEFT,   speed);
    motor.dcMotorRun(MOTOR_RIGHT,  speed);
    break;
  case BACKWARD:
    motor.dcMotorRun(MOTOR_LEFT,  -speed);
    motor.dcMotorRun(MOTOR_RIGHT, -speed);
    break;
  }
}

uint8_t decodeSignal() {
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
    return LEFT;
  case 0x02FD02FD:
    return RIGHT;
  case 0x02FD9867:
    return FORWARD;
  case 0x02FDB847:
    return BACKWARD;
  default:
    return STOPPED;
  }
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  IR.Init(PIN_IRDA);

  motor.init();
  performMove();
}

void loop() {
  if (IR.IsDta()) {
    IR.Recv(irMessage);
    advanceState(decodeSignal());
    performMove();
  }
}
