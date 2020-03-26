#include <Arduino.h>
#include <Ultrasonic.h>

const uint8_t pinEcho = 8;
const uint8_t pinBuzz = 3;
const int maxDistCm = 255;

Ultrasonic sensor(pinEcho);

void setup() {
  Serial.begin(9600);
  pinMode(pinBuzz, OUTPUT);
  pinMode(pinEcho, INPUT);
}


long distance() {
  return sensor.MeasureInCentimeters();
}

void loop() {
  int dist = distance();
  if (dist >= 0) {
    Serial.println(dist);
    digitalWrite(pinBuzz, dist < 50 ? HIGH : LOW);
  }

  delay(50);
}
