#include <Arduino.h>
const uint8_t LED_LL = 6;
const uint8_t LED_L = 7;
const uint8_t LED_C= 8;
const uint8_t LED_R = 9;
const uint8_t LED_RR = 10;

const uint16_t sleepTime = 50;
const uint16_t userTime = 300;
const byte calibrationTimes = 20;

int left = 0;
int right = 0;
int diff = 0;
int calibration = 0;

void blink(uint8_t led) {
  digitalWrite(led, HIGH);
  delay(sleepTime);
  digitalWrite(led, LOW);
}

void turnOnIf(uint8_t led, uint8_t onlyOnLed) {
  digitalWrite(led, led == onlyOnLed ? HIGH : LOW);
}

void turnOnOnly(uint8_t led) {
  turnOnIf(LED_LL, led);
  turnOnIf(LED_L, led);
  turnOnIf(LED_C, led);
  turnOnIf(LED_R, led);
  turnOnIf(LED_RR, led);
}

void sayHi() {
  blink(LED_LL);
  blink(LED_L);
  blink(LED_C);
  blink(LED_R);
  blink(LED_RR);
  delay(userTime);
}

void readDiff() {
  left = map(analogRead(A5), 0, 1023, 0, 1000);
  right = map(analogRead(A4), 0, 1023, 0, 1000);
  diff = left - right - calibration;
}

void calibrate() {
  int sum = 0;
  for (byte i = 0; i < calibrationTimes; i++) {
    readDiff();
    sum += diff;
    blink(LED_C);
    delay(25);
  }
  calibration = sum / calibrationTimes;
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_LL, OUTPUT);
  pinMode(LED_L, OUTPUT);
  pinMode(LED_C, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_RR, OUTPUT);

  turnOnOnly(0); // turn off all
  sayHi();
  calibrate();
  sayHi();
}


void loop() {
  readDiff();
  Serial.println(diff);

  if (diff >= 500) {
    turnOnOnly(LED_LL);
  } else if (diff >= 100) {
    turnOnOnly(LED_L);
  } else if (diff > -100) {
    turnOnOnly(LED_C);
  } else if (diff > -500) {
    turnOnOnly(LED_R);
  } else if (diff <= -500) {
    turnOnOnly(LED_RR);
  }

  delay(20);
}
