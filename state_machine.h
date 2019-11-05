
#ifndef state_machine_H_
#define state_machine_H_

#include<stdlib.h>
#include <stdio.h>
#include "I2C_PES_PROJECT4_1.h"
#include <stdint.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_common.h"
#include "MKL25Z4.h"
#include <fsl_debug_console.h>

typedef enum
{
    Sucess,
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




static volatile int ut_f = 0;

static volatile int db_f = 1;

//#define ALERT I2C0->SMB= I2C0->SMB |= I2C_SMB_ALERTEN_MASK
//Global Flag Declarations
static volatile int counter2=0;
static volatile int l_f = 0;                    //Logger Flag , Set when logger is on or enable
static volatile int sm_f = 0;					//State Machine Flag    0 when State machine 1 , 1 when state machine 2
static volatile int al_f = 0;					//Alert flag            1 when temperature under threshold
static volatile int nack_f = 0;					//No_Acknowledgment Flag  default 0   , set to 1 when disconnected or no acknowledgment received
static volatile int timeout_val =1;             //value of timeout



void delay(volatile int32_t number);
void blink(int LED_no, int delay_counter);
void Handle_Temp_Read();
void Handle_Average_Wait();
void Handle_Temperature_Alert();
void Handle_Disconnect();
void POST();
void state_machines();


#endif
