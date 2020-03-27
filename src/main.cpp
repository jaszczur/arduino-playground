#include <Arduino.h>
#include <Grove_Motor_Driver_TB6612FNG.h>

MotorDriver motor;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  motor.init();
  motor.dcMotorStop(MOTOR_CHA);
  motor.dcMotorStop(MOTOR_CHB);
  delay(5000);
}


void loop() {
  Serial.println("run at speed=200");
  motor.dcMotorRun(MOTOR_CHA, 200);
  motor.dcMotorRun(MOTOR_CHB, 200);
  delay(1000);

  Serial.println("brake");
  motor.dcMotorBrake(MOTOR_CHA);
  motor.dcMotorBrake(MOTOR_CHB);
  delay(1000);

  Serial.println("run at speed=-100");
  motor.dcMotorRun(MOTOR_CHA, -100);
  motor.dcMotorRun(MOTOR_CHB, -100);
  delay(700);
  Serial.println("rotate at speed=100");
  motor.dcMotorRun(MOTOR_CHA, -100);
  motor.dcMotorRun(MOTOR_CHB, 100);
  delay(1000);

  Serial.println("stop");
  motor.dcMotorStop(MOTOR_CHA);
  motor.dcMotorStop(MOTOR_CHB);
  delay(1000);
}
