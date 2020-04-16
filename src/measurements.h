#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <Arduino_FreeRTOS.h>
#include <queue.h>

typedef struct MeasurementsConfig {
  int moisturePin;
} MeasurementsConfig_t;

void measurements_init(MeasurementsConfig_t *config);
int measurements_read_moisture();

#endif /* MEASUREMENTS_H */
