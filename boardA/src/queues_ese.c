#include "../include/queues_ese.h"

/* Safe for global use, only written once and only ever read */
QueueHandle_t i2c2Q = NULL;
QueueHandle_t eeprom_logQ = NULL;
