#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#include "ledbar.h"
#include "measurements.h"

#define LED_PIN 2
#define LED_COUNT 10
#define BRIGHTNESS 25
#define SWITCH_PIN 8
#define MOIST_PIN 3

void setup() {
  Serial.begin(115200);
  pinMode(SWITCH_PIN, INPUT);

  LedbarConfig_t ledConf = {
      .pin = LED_PIN,
      .count = LED_COUNT,
      .brightness = BRIGHTNESS,
      .shouldReadMoisture = []() { return digitalRead(SWITCH_PIN); }
  };
  ledbar_initialize(&ledConf);

  MeasurementsConfig_t measConf = {
      .moisturePin = MOIST_PIN,
  };
  measurements_init(&measConf);

  ledbar_task_start();
}

void loop() {}
