#include <Arduino.h>

#include "measurements.h"

static int moisturePin = -1;

int measurements_read_moisture() { return map(analogRead(moisturePin), 0, 600, 0, 100); }

void measurements_init(MeasurementsConfig_t *config) {
  moisturePin = config->moisturePin;
}
