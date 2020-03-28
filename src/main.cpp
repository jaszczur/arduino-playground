#include <Arduino.h>
#include <Grove_Motor_Driver_TB6612FNG.h>
#include <IRSendRev.h>
#include <Ultrasonic.h>


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
#define DEFAULT_SPEED  125
#define SPEED_STEP     25
#define MIN_DIST_CM    20

#define STOPPED  0
#define LEFT     1
#define RIGHT    2
#define FORWARD  3
#define BACKWARD 4
#define NOOP     5

unsigned char irMessage[24];
uint8_t state = STOPPED;
uint8_t action = NOOP;
uint16_t distance = 0;
uint8_t speed = DEFAULT_SPEED;
MotorDriver motor;
Ultrasonic ultrasonic(PIN_SONIC_TRIG, PIN_SONIC_ECHO);

void advanceState() {
  if ((state == FORWARD && distance < MIN_DIST_CM)
      || (state == BACKWARD && action == FORWARD)
      || (state == FORWARD  && action == BACKWARD)) {
    Serial.println("stop");
    state = STOPPED;
    speed = DEFAULT_SPEED;
  } else if (action == BACKWARD || action == FORWARD) {
    Serial.println("moar");
    state = action;
    speed = min(speed + SPEED_STEP, 254);
  } else if (action == LEFT || action == RIGHT) {
    speed = DEFAULT_SPEED;
    state = action;
  } else {
    Serial.println("rest");
  }
}

void performMove() {
  switch (state) {
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
    action = LEFT;
    break;
  case 0x02FD02FD:
    action = RIGHT;
    break;
  case 0x02FD9867:
    action = FORWARD;
    break;
  case 0x02FDB847:
    action = BACKWARD;
    break;
  default:
    action = STOPPED;
    break;
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
  distance = ultrasonic.read();
  if (IR.IsDta()) {
    IR.Recv(irMessage);
    decodeSignal();
  } else {
    action = NOOP;
  }
  advanceState();
  performMove();
}
