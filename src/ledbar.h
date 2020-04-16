#ifndef LEDBAR_H
#define LEDBAR_H

#include "queue.h"

typedef struct LedbarConfig {
  int pin;
  int count;
  int brightness;
  int (*shouldReadMoisture)();
} LedbarConfig_t;

void ledbar_initialize(LedbarConfig_t *config);
void ledbar_destroy();
void ledbar_task_start();

#endif /* LEDBAR_H */
