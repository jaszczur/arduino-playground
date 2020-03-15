#include <Arduino.h>
const uint8_t LED_RED = 8;
const uint8_t LED_YELLOW = 9;
const uint8_t LED_GREEN= 10;
const uint8_t LED_BLUE = 11;
const uint8_t SWITCH = 2;

const uint16_t sleepTime = 50;
const uint16_t userTime = 300;

void blink(uint8_t led) {
  digitalWrite(led, HIGH);
  delay(sleepTime);
  digitalWrite(led, LOW);
}

void turnOnIf(uint8_t led, uint8_t onlyOnLed) {
  digitalWrite(led, led == onlyOnLed ? HIGH : LOW);
}

void turnOnOnly(uint8_t led) {
  turnOnIf(LED_RED, led);
  turnOnIf(LED_YELLOW, led);
  turnOnIf(LED_GREEN, led);
  turnOnIf(LED_BLUE, led);
}

void sayHi() {
  blink(LED_BLUE);
  delay(userTime);
}

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(SWITCH, INPUT_PULLUP);

  turnOnOnly(0); // turn off all
}

void waitForSwitch() {
  delay(userTime);
  while(digitalRead(SWITCH) == HIGH) {}
}

void loop() {
  turnOnOnly(LED_RED);
  sayHi();
  waitForSwitch();
  turnOnOnly(LED_YELLOW);
  delay(userTime * 3);
  turnOnOnly(LED_GREEN);
  delay(userTime * 8);
  turnOnOnly(LED_YELLOW);
  delay(userTime * 3);
}
