#ifndef _LOGGER_H_
#define _LOGGER_H_
#include <stdint.h>                              //required .h files
#include "logger.h"
#include "board.h"

void delay(volatile int32_t number);
void blink(int LED_no, int delay_counter);          //blinking led functions


#endif
