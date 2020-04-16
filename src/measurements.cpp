#include <Arduino.h>

#include "measurements.h"

static int moisturePin = -1;

int measurements_read_moisture() { return analogRead(moisturePin); }

void measurements_init(MeasurementsConfig_t *config) {
  moisturePin = config->moisturePin;
}
