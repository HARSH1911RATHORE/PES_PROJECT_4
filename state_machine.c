#include "state_machine.h"
#include "I2C_PES_PROJECT4_1.h"
#include "uCUnit.h"
#include "system.h"

struct sStateTableEntry stateTable[]=
{
    {TEMP_READ, AVERAGE_WAIT, TEMP_ALERT, DISCONNECT, dont_care, dont_care},    //Temp Read
    {AVERAGE_WAIT, dont_care, dont_care, DISCONNECT, TEMP_READ, dont_care},	//Average Wait
    {TEMP_ALERT, AVERAGE_WAIT, dont_care, DISCONNECT, dont_care,dont_care},	//Alert State
    {DISCONNECT, dont_care, dont_care, dont_care, dont_care, dont_care},	//Disconnect
};

int timeout_delay = 2000;     						//1 sec
int Prev_temp = 25;								//stores the last value of temperature to average
state_SM1 Current_state_SM1;
state_SM1 Current_state_SM2;

void delay(volatile int32_t number)
{

    int multiplier = 4000;               //for the Mhz system clock
    int delay = number*multiplier;
    while (delay!=0)                    //perform desired delay with the following loop
    {
        __asm volatile ("nop");
        delay--;
    }
}


void blink(int LED_no, int delay_counter)
{


    if (LED_no==0)              //LED is Red
    {
        LED_RED_ON();
        delay(delay_counter);   //give a delay to the led
        LED_RED_OFF();
    }

    if (LED_no==1)               //LED is Green
    {
        LED_GREEN_ON();
        delay(delay_counter);    //give a delay to the led
        LED_GREEN_OFF();
    }

    if (LED_no==2)               //LED is Blue
    {
        LED_BLUE_ON();
        delay(delay_counter);  //give a delay to the led
        LED_BLUE_OFF();
    }
}


void Handle_Temp_Read()
{
    int temperature_read=i2c_read_bytes(addr,temp_addr,0);

    PRINTF("\n\r TEMPERATURE READ=%d\n\r",temperature_read);

    if (nack_f == 1)      //Disconnected
    {
        Current_state_SM2 = stateTable[3].CURRENT_STATE;
        PRINTF("\n\r");
        if (ut_f==1)                 //check for the unit test flag
        {
            PRINTF("\n\r");
            UCUNIT_CheckIsEqual(nack_f,1);  //perform unit test
            PRINTF("\n\r");
        }
        PRINTF("\n\r");
    }
    if (al_f == 1)        //Temperature Alert
    {
        Current_state_SM2 = stateTable[2].CURRENT_STATE;
    }
    else                  //Go to Average Wait State
    {
        Current_state_SM2 = stateTable[1].CURRENT_STATE;
    }
}


void Handle_Average_Wait()
{
    int i;
    int sum=0;
    int average;
    int data[5];


    data[timeout_val]=i2c_read_bytes(addr,temp_addr,0);    //put the data into an array to perform average



    for (i=0; i<=timeout_val; i++)
    {
        sum=sum+data[i];
        //     PRINTF("\n\rSUM=%d",sum);
    }
    average=(float)sum/(timeout_val+1);
    PRINTF("\n\rCURRENT TEMPERATURE=%d\n\r",data[timeout_val]);
    PRINTF("\n\rAVERAGE TEMPERATURE=%f\n\r",average);


    if (nack_f == 1)      //Disconnected
    {
        Current_state_SM2 = stateTable[3].CURRENT_STATE;
        PRINTF("\n\r");
        if (ut_f==1)						//check for the unit test flag
        {
            PRINTF("\n\r");
            UCUNIT_CheckIsEqual(nack_f,1);  //perform unit test
            PRINTF("\n\r");
        }
        PRINTF("\n\r");
    }
    else					//Temperature read operation
    {
        Current_state_SM2 = stateTable[0].CURRENT_STATE;
        timeout_val++;
        delay(timeout_delay);
    }

}

void Handle_Temperature_Alert()
{
    al_f =0;     	      //RESET the alert Flag
    if (nack_f == 1)      //Disconnected
    {
        Current_state_SM2 = stateTable[3].CURRENT_STATE;
        PRINTF("\n\r");
        if (ut_f==1)
        {
            PRINTF("\n\r");
            UCUNIT_CheckIsEqual(nack_f,1); //check for the unit test flag
            PRINTF("\n\r");
        }
        PRINTF("\n\r");
    }
    else
    {
        Current_state_SM2 = stateTable[1].CURRENT_STATE;
    }
}

void Handle_Disconnect()
{
    if (db_f == 1)                    //check for debug flag to glow the leds
    {
        blink(1,1000);               //blink the red led for disconnected state
    }
    PRINTF("\n\n\r--------------------------------- Sensor Disconnected-----------------------\n\n\r ");
    exit (0);
}

void POST()
{
    i2c_read_bytes(addr,temp_addr,2);
    int i;
    PRINTF("\n\n\r-------------------------Power on Self test--------------------------------------------\n\r");
    //Call I2c here
    if (nack_f == 0)
    {

        for (i=0; i<=3; i++)
        {
            if (db_f == 1)      //check for debug flag to glow the leds
            {
                blink(0,250);   //for post operation blink group of rgb
                //leds to signify initialization
                blink(1,250);
                blink(2,250);
            }
        }
        PRINTF("\n\n \n\r-------------------POST Successful----------------------------------------------\n\r");
    }
    else
    {
        Handle_Disconnect();
    }

}

void state_machines()
{

    POST();

    //  State_Machine_1:                     //state machine 1 label
    while(1)
    {
        //state machine 1 label
        PRINTF("\n\r State Machine 1\n\r");



        //State Machine Initializations
        Current_state_SM1 = TEMP_READ;     //Default starting state

        sm_f = 0;                        //Set State Machine to 1
        timeout_val = 0;				  //Reset Timeout value to 1
        while(timeout_val < 4  )
        {



            switch(Current_state_SM1)
            {
            case TEMP_READ:
                PRINTF("\n\r The sensor is in TEMP_READ STATE\n\r");
                Handle_Temp_Read();
                					//    Read temperature here and set flags
                if (db_f == 1)
                {
                    blink(1,1000);
                }

                if (nack_f == 1)      //Disconnected
                {
                    Current_state_SM1 = DISCONNECT;
                    PRINTF("\n\r");
                    if (ut_f==1)                   //check for the unit test flag
                    {
                        PRINTF("\n\r");
                        UCUNIT_CheckIsEqual(nack_f,1);
                        PRINTF("\n\r");
                    }
                    PRINTF("\n\r");
                    break;
                }
                if (al_f == 1)        //Temperature Alert
                {
                    Current_state_SM1 = TEMP_ALERT;   //the transition will be to Temperature Alert
                    PRINTF("\n\r");
                    if (ut_f==1)				//check for the unit test flag
                    {
                        PRINTF("\n\r");
                        UCUNIT_CheckIsEqual(al_f,1);
                        PRINTF("\n\r");
                    }
                    PRINTF("\n\r");
                    break;
                }
                else                  //Normal Temperature read
                {
                    PRINTF("\n\r");
                    if (ut_f==1)
                    {
                        PRINTF("\n\r");              //unit test
                        UCUNIT_CheckIsEqual(Current_state_SM1,TEMP_READ);
                        PRINTF("\n\r");
                    }
                    PRINTF("\n\r");
                    Current_state_SM1 = AVERAGE_WAIT;
                    break;
                }

            case AVERAGE_WAIT:

                if (db_f == 1)
                {
                    blink(1,1000);
                }
                PRINTF("\n\r The sensor is in AVERAGE_WAIT STATE\n\r");
                Handle_Average_Wait();
                if (nack_f == 1)      //Disconnected
                {
                    Current_state_SM1 = DISCONNECT;
                    PRINTF("\n\r");                //the transition will be to Disconnected
                    if (ut_f==1)
                    {
                        PRINTF("\n\r");      //unit test
                        UCUNIT_CheckIsEqual(nack_f,1);
                        PRINTF("\n\r");
                    }
                    PRINTF("\n\r");
                    break;
                }
                else
                {
                    PRINTF("\n\r");  //else go into the average/wait state
                    if (ut_f==1)      //check for the unit test flag
                    {
                        PRINTF("\n\r");			//unit test
                        UCUNIT_CheckIsEqual(Current_state_SM1,AVERAGE_WAIT);
                        PRINTF("\n\r");
                    }
                    PRINTF("\n\r");
                    Current_state_SM1 = TEMP_READ;    //the transition will be to temp read
                    timeout_val++;
                    delay(timeout_delay);
                    break;
                }
            case TEMP_ALERT:

                if (db_f == 1)
                {
                    blink(2,1000);
                }
                PRINTF("\n\r The sensor is in TEMP_ALERT STATE\n\r");
                Handle_Temperature_Alert();
                al_f =0;     	      //RESET the alert Flag
                if (nack_f == 1)      //Disconnected
                {
                    Current_state_SM1 = DISCONNECT;      //the transition will be to disconnected
                    if (ut_f==1)
                    {
                        PRINTF("\n\r");
                        UCUNIT_CheckIsEqual(nack_f,1);   //carry out unit test
                        PRINTF("\n\r");
                    }
                    break;
                }
                else
                {
                    Current_state_SM1 = AVERAGE_WAIT;    //the transition will be to average wait
                    break;
                }
            case dont_care:      //for states of no use no operation is performed
            {
                break;
            }

            case Timeout3:     //for states of no use no operation is performed
            {
                break;
            }
            case Timeout4:    //for states of no use no operation is performed
            {
                break;
            }

            case DISCONNECT:   //for disconnect state check the debug flag
                //and glow red led
                if (db_f == 1)
                {
                    blink(0,1000);
                }
                Handle_Disconnect();  //go to disconnect function


            }

        }
////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////State Machine 2//////////////////////////////////////////
        timeout_val = 0;
        Current_state_SM2 = TEMP_READ;       //initializing first state
        PRINTF("\n \n\r State Machine 2\n\r");


        while (timeout_val < 4 )
        {
            if (Current_state_SM2 == stateTable[0].CURRENT_STATE)
            {
                PRINTF("\n \n\rWe are in Temperature read state\n\r");
                Handle_Temp_Read();
                if (db_f == 1)           //check for the debug status and blink led
                {
                    blink(1,1000);
                }
                if (nack_f==1)            //check for no ack in the read function and got to disconnect
                {
                    Handle_Disconnect();
                }

            }
            if (Current_state_SM2 == stateTable[1].CURRENT_STATE)
            {
                PRINTF("\n\n\r We are in Temperature Wait Average state\n\r");
                Handle_Average_Wait();
                if (db_f == 1)         //check for the debug status and blink led
                {
                    blink(1,1000);
                }
                if (nack_f==1)      //check for no ack in the read function and go to disconnect
                {
                    Handle_Disconnect();
                }

            }
            if (Current_state_SM2 == stateTable[2].CURRENT_STATE)
            {
                PRINTF("\n\r We are in Temperature Temperature Alert state\n\r");
                Handle_Temperature_Alert();
                if (db_f == 1)      //check for the debug status and blink led
                {
                    blink(2,1000);
                }
                if (nack_f==1)    //check for no ack in the read function and go to disconnect
                {
                    Handle_Disconnect();
                }

            }
            if (Current_state_SM2 == stateTable[3].CURRENT_STATE)
            {
                PRINTF("\n\n\r We are in Disconnect state\n\r");
                Handle_Disconnect();
                if (db_f == 1)        //check for the debug status and blink led
                {
                    blink(0,1000);
                }
                if (nack_f==1)     //check for no ack in the read function and go to disconnect
                {
                    Handle_Disconnect();
                }

            }

        }

    }

    PRINTF("\n\r");
    if (ut_f==1)
    {
        PRINTF("\n\r");
        UCUNIT_TestcaseEnd();
        PRINTF("\n\r");
        UCUNIT_WriteSummary();
        PRINTF("\n\r");
        UCUNIT_Shutdown();
        PRINTF("\n\r");
    }
    PRINTF("\n\r");

}
