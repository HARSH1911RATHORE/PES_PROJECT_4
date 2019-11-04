/*
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    State_Machine_1.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */

///////////////////Changes Here//////////////////////////////////////////
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

struct sStateTableEntry stateTable[]={
                		{TEMP_READ, AVERAGE_WAIT, TEMP_ALERT, DISCONNECT, dont_care, dont_care},    //Temp Read
        				{AVERAGE_WAIT, dont_care, dont_care, DISCONNECT, TEMP_READ, dont_care},	//Average Wait
        				{TEMP_ALERT, AVERAGE_WAIT, dont_care, DISCONNECT, dont_care ,dont_care},	//Alert State
        				{DISCONNECT, dont_care, dont_care, dont_care, dont_care, dont_care},	//Disconnect
                };



//Global Flag Declarations
static volatile int l_f = 0;                    //Logger Flag , Set when logger is on or enable
static volatile int sm_f = 0;					//State Machine Flag    0 when State machine 1 , 1 when state machine 2
static volatile int al_f = 0;					//Alert flag            1 when temperature under threshold
static volatile int nack_f = 0;					//No_Acknowledgment Flag  default 0   , set to 1 when disconnected or no acknowledgment received
static volatile int timeout_val =1;             //value of timeout
int timeout_delay = 2000;     						//1 sec
int Prev_temp = 25;								//stores the last value of temerature to average
state_SM1 Current_state_SM1;
state_SM1 Current_state_SM2;


void delay(volatile int32_t number)
		{

			int multiplier = 4000;     //for the Mhz system clock
			int delay = number*multiplier;
			while (delay!=0)
					{
						__asm volatile ("nop");
						delay--;
					}
		}


void blink(int LED_no, int delay_counter)
			{


				if (LED_no==0) //LED is Red
					{
						LED_RED_ON();
						delay(delay_counter);
						LED_RED_OFF();
					}

				if (LED_no==1) //LED is Green
					{
						LED_GREEN_ON();
						delay(delay_counter);
						LED_GREEN_OFF();
					}

				if (LED_no==2) //LED is Blue
						{
							LED_BLUE_ON();
							delay(delay_counter);
							LED_BLUE_OFF();
						}
			}


void Handle_Temp_Read()
{
					 if (nack_f == 1)      //Disconnected
	                	{Current_state_SM2 = stateTable[3].CURRENT_STATE;
	                	}
	                 if (al_f == 1)        //Temperature Alert
	                    {Current_state_SM2 = stateTable[2].CURRENT_STATE;
	                    }
	                 else                  //Go to Average Wait State
	                	 {Current_state_SM2 = stateTable[1].CURRENT_STATE;
	                    }
}


void Handle_Average_Wait()
{

					if (nack_f == 1)      //Disconnected
	            	   {Current_state_SM2 = stateTable[3].CURRENT_STATE;
	            	   }
	                else					//Temperature read operation
	                	{Current_state_SM2 = stateTable[0].CURRENT_STATE;
	                	timeout_val++;
	                	delay(timeout_delay);
	                	}

}

void Handle_Temperature_Alert()
{
					al_f =0;     	      //RESET the alert Flag
	            	if (nack_f == 1)      //Disconnected
	            	   {Current_state_SM2 = stateTable[3].CURRENT_STATE;
	            	   }
	            	else
	            		{
	            		Current_state_SM2 = stateTable[1].CURRENT_STATE;
	            		}
}

void Handle_Disconnect()
{
		printf("\n--------------------------------- Sensor Disconnected-----------------------\n ");
}

void POST()
{
	int i;
	printf("\n-------------------------Power on Self test--------------------------------------------");
	//Call I2c here
	if (nack_f == 0)
	{

		for (i=0;i<=5;i++)
		{
			blink(0,250);
			blink(1,250);
			blink(2,250);
		}
		printf("\n\n \n -------------------POST Sucessful----------------------------------------------");
	}

}






int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();
    //Initialize the LEDs and GPIO
        LED_RED_INIT(1);
    	LED_GREEN_INIT(1);
    	LED_BLUE_INIT(1);

    PRINTF("Hello World\n");
    POST();


    /////////////////////////////////////changes here//////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////



    //////////////////////////state machine 1///////////////////////////////////////////////
    int counter =0;




    State_Machine_1:                     //state machine 1 label
    printf("\n \n State Machine 1");



    //State Machine Initializations
    Current_state_SM1 = TEMP_READ;     //Default starting state
                    //Testing variable
    sm_f = 0;                        //Set State Machine to 1
    timeout_val = 0;				  //Reset Timeout value to 1
    while(timeout_val < 4)
    {

    	//State testing code
    	counter++;
    	if (counter ==15)
    	{nack_f =1;}
    	if (counter ==7)
    	    {al_f =1;}

        switch(Current_state_SM1)
        {
            case TEMP_READ:
            	PRINTF("\n The sensor is in TEMP_READ STATE");
            	//    Read temperature here and set flags
            	blink(1,1000);

            	if (nack_f == 1)      //Disconnected
                	{Current_state_SM1 = DISCONNECT;
                	break;}
                 if (al_f == 1)        //Temperature Alert
                    {Current_state_SM1 = TEMP_ALERT;
                    break;}
                 else                  //Normal Temperature read
                    {Current_state_SM1 = AVERAGE_WAIT;
                    break;}

            case AVERAGE_WAIT:
            	blink(1,1000);
            	PRINTF("\n The sensor is in AVERAGE_WAIT STATE");
            	if (nack_f == 1)      //Disconnected
            	   {Current_state_SM1 = DISCONNECT;
            	   break;}
                else
                	{Current_state_SM1 = TEMP_READ;
                	timeout_val++;
                	delay(timeout_delay);
                	break;}
            case TEMP_ALERT:
            	blink(2,1000);
            	PRINTF("\n The sensor is in TEMP_ALERT STATE");
            	al_f =0;     	      //RESET the alert Flag
            	if (nack_f == 1)      //Disconnected
            	   {Current_state_SM1 = DISCONNECT;
            	   break;}
            	else
            		{
            		Current_state_SM1 = AVERAGE_WAIT;
            		break;}
            case DISCONNECT:
            	 blink(0,1000);
            	 goto Disconnect_END;

        }

    }
    ////////////////////////////////////////////////////////////////////////////////////////



    //////////////////////////////State Machine 2//////////////////////////////////////////
    timeout_val = 0;
    Current_state_SM2 = TEMP_READ;       //initializing first state
    printf("\n \n State Machine 2");
    //printf("\n%d",stateTable[0].goEvent);

    while (timeout_val < 4)
    {


    	//test codes to check working of the flags
    	counter++;
		if (counter == 7)
		{al_f  = 1;}



    	if (Current_state_SM2 == stateTable[0].CURRENT_STATE)
    			{
    				PRINTF("\n We are in Temperature read state");
    				Handle_Temp_Read();
    				blink(1,1000);

    			}
    	if (Current_state_SM2 == stateTable[1].CURRENT_STATE)
    	    	{
    	    		PRINTF("\n We are in Temperature Wait Average state");
    	    		Handle_Average_Wait();
    	    		blink(1,1000);

    	    	}
    	if (Current_state_SM2 == stateTable[2].CURRENT_STATE)
    	    	{
    	    		PRINTF("\n We are in Temperature Temperature Alert state");
    	    		Handle_Temperature_Alert();
    	    		blink(2,1000);

    	    	}
    	if (Current_state_SM2 == stateTable[3].CURRENT_STATE)
    	    	{
    	    		PRINTF("\n We are in Disconnect state");
    	    		Handle_Disconnect();
    	    		blink(0,1000);
    	    		goto Disconnect_END;


    	    	}

    }


    //go back to state machine 1
    goto State_Machine_1;






    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
//    while(1) {
//        i++ ;
//        /* 'Dummy' NOP to allow source level single stepping of
//            tight while() loop */
//        __asm volatile ("nop");
//    }




    Disconnect_END:
    printf("\n The Temperature Sensor has been Disconnected");




    return 0 ;
}
