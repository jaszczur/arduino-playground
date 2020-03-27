#include <Arduino.h>
#include <Grove_Motor_Driver_TB6612FNG.h>
#include <IRSendRev.h>

MotorDriver motor;


void setupMotor() {
  Wire.begin();
  Serial.begin(9600);
  motor.init();
  motor.dcMotorStop(MOTOR_CHA);
  motor.dcMotorStop(MOTOR_CHB);
  delay(5000);
}


void loopMotor() {
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

#define BIT_LEN       0
#define BIT_START_H   1
#define BIT_START_L   2
#define BIT_DATA_H    3
#define BIT_DATA_L    4
#define BIT_DATA_LEN  5
#define BIT_DATA      6

unsigned char data[24];

void setupReceiver() {
  Serial.begin(115200);
  IR.Init(2);
}


void loopReceiver() {
  if(IR.IsDta()) {
    IR.Recv(data);

    Serial.print("{:len ");
    Serial.println(data[BIT_LEN]);

    Serial.print(" :start-h ");
    Serial.println(data[BIT_START_H]);

    Serial.print(" :start-l ");
    Serial.println(data[BIT_START_L]);

    Serial.print(" :data-h ");
    Serial.println(data[BIT_DATA_H]);

    Serial.print(" :data-l ");
    Serial.println(data[BIT_DATA_L]);

    Serial.print(" :data [");
    for (int i = BIT_DATA; i < BIT_DATA + data[BIT_DATA_LEN]; i++) {
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.println("]\n}\n------------------------------------");
  }
}

void setup() {
  setupReceiver();
}

void loop() {
  loopReceiver();
}
