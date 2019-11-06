
#ifndef _LOGGER_H_
#define _LOGGER_H_
#include<stdint.h>
#include <stdio.h>
#include "state_machine.h"
#include "I2C_PES_PROJECT4_1.h"
//typedef enum  {
//	state_machine1,
//	TEMP_READ1,
//	AVERAGE_WAIT1,
//	TEMP_ALERT1,
//	state_machine2,
//	TEMP_READ2,
//	AVERAGE_WAIT2,
//	TEMP_ALERT2
//
//}state_machine_log;

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



//void logger(log_level level,functions function,char message);
//void log_i2c_read(int config,int data_buf[3]);
//void log_i2c_write();
void log_blink_led(int check_led);
void log_Handle_Temp_Read(int temperature_read);
void log_Handle_Average_Wait(int8_t current_data,int8_t AVERAGE);
void log_Handle_Disconnect();
void log_POST(int check_post);
void log_state_machines(int check_state_machine);
void log_next_line();

#endif
