#include <Arduino.h>
const uint8_t LED_EXT = 12;
const uint16_t baseTime = 1000;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_EXT, OUTPUT);
}

void basic() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(baseTime);
  digitalWrite(LED_BUILTIN, LOW);
}

void fancy() {
  for (uint16_t i = 0; i < 20; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(i * baseTime/100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(i * baseTime/200);
  }
}

void loop() {
  digitalWrite(LED_EXT, HIGH);
  basic();
  // fancy();
  digitalWrite(LED_EXT, LOW);
  delay(baseTime);
}
