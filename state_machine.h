
#ifndef state_machine_H_
#define state_machine_H_

#include<stdlib.h>
#include <stdio.h>

#include <stdint.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_common.h"
#include "MKL25Z4.h"
#include <fsl_debug_console.h>
#include <TMP102.h>
#include "uCUnit.h"
#include "system.h"
#include "logger.h"
#include "led.h"


typedef enum
{
    Sucess,										//enum for various test operations
    Memory_Fault,
    other_errors
} Error_codes;


typedef enum
{
    TEMP_READ,
    AVERAGE_WAIT,
    TEMP_ALERT,
    DISCONNECT,
    Timeout3,
    Timeout4,
    dont_care
} state_SM1;

struct sStateTableEntry
{
    state_SM1 CURRENT_STATE;       //the name of the primary state
    state_SM1 goEvent;           //every state has a go event
    state_SM1 AlertEvent;		 //every state has a Alert event
    state_SM1 DisconnectEvent;	 //every state has a Disconnect event
    state_SM1 Timeout3;			 //every state has a Timeout3 event
    state_SM1 Timeout4;			 //every state has a Timeout4 event
};




static volatile int ut_f = 1;                //flag to perform unit test  //if same flag is made 0 status messages are printed

static volatile int db_f = 1;				//flag to perform debug

//Global Flag Declarations

static volatile int l_f = 0;                    //Logger Flag , Set when logger is on or enable
static volatile int sm_f = 0;					//State Machine Flag    0 when State machine 1 , 1 when state machine 2
static volatile int al_f = 0;					//Alert flag            1 when temperature under threshold
static volatile int nack_f = 0;					//No_Acknowledgment Flag  default 0   , set to 1 when disconnected or no acknowledgment received
static volatile int timeout_val =1;             //value of timeout


int state_machines();
void delay(volatile int32_t number);              //function prototype to perform various state machine operations
void blink(int LED_no, int delay_counter);
int Handle_Temp_Read();
int Handle_Average_Wait();                       //all handle functions to perform various operations
int Handle_Temperature_Alert();
int Handle_Disconnect();
void wait_to_complete(void);
int i2c_read_bytes_avg(uint8_t dev_adx,uint8_t reg_adx);
void log_blink_led(int check_led);
void log_Handle_Average_Wait(int8_t current_data,int8_t AVERAGE);   //all log functions  to perform various print operations
void log_Handle_Disconnect();
void log_POST(int check_post);
void log_state_machines(int check_state_machine);
void log_next_line();
void log_i2c_write_bytes();                            //log functions for i2c and, led and state machine
void log_Handle_Temp_Read();
void log_Handle_Temperature_Alert();
void log_alert(int alert_check);

int POST();



#endif
