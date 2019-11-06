
#ifndef _LOGGER_H_
#define _LOGGER_H_
#include<stdint.h>
#include <stdio.h>
#include "state_machine.h"
#include "I2C_PES_PROJECT4_1.h"


enum{
status,
test,
debug
}log_level;

enum{

	blink_led,
	Handle_Temperature_Read,
	Handle_Avg_Wait,
	Handle_Temp_Alert,
	Handle_Disc,
	Power_start,
	state_mach,

	i2c_write,
	i2c_read

}function_type;




void log_blink_led(int check_led);

void log_Handle_Average_Wait(int8_t current_data,int8_t AVERAGE);
void log_Handle_Disconnect();
void log_POST(int check_post);
void log_state_machines(int check_state_machine);
void log_next_line();
void log_i2c_write_bytes();
void log_Handle_Temp_Read(int temperature_read);

#endif
