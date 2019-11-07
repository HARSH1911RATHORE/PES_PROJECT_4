#include "state_machine.h"
#include<stdlib.h>
#include "logger.h"
#include "TMP102.h"
#include "led.h"

struct sStateTableEntry stateTable[]=
{
    {TEMP_READ, AVERAGE_WAIT, TEMP_ALERT, DISCONNECT, dont_care, dont_care},    //Temp Read
    {AVERAGE_WAIT, dont_care, dont_care, DISCONNECT, TEMP_READ, dont_care},	//Average Wait
    {TEMP_ALERT, AVERAGE_WAIT, dont_care, DISCONNECT, dont_care,dont_care},	//Alert State
    {DISCONNECT, dont_care, dont_care, dont_care, dont_care, dont_care},	//Disconnect
};

int8_t AVERAGE=0;

int timeout_delay = 2000;     						//1 sec
int Prev_temp = 25;								//stores the last value of temperature to average
state_SM1 Current_state_SM1;
state_SM1 Current_state_SM2;

int Handle_Temp_Read()
{
    int temperature_read=i2c_read_bytes(addr,temp_addr,0);


    if (temperature_read==1)
    {
        return 1;
    }
    else if (temperature_read==5)
    {
    	Current_state_SM2 = stateTable[2].CURRENT_STATE;
        return 5;
    }


    log_Handle_Temp_Read();


    if (nack_f == 1)            //Disconnected //check for nack_flag which is an disconnect flag
    {
        Current_state_SM2 = stateTable[3].CURRENT_STATE;
        log_next_line();

        if (ut_f==1)                                                 //check for the unit test flag
        {
            log_next_line();

            UCUNIT_CheckIsEqual(nack_f,1);                       //perform unit test
            log_next_line();

        }
        log_next_line();

    }
    if (al_f == 1)                                        //Temperature Alert
    {
        Current_state_SM2 = stateTable[2].CURRENT_STATE;
    }
    else                                             //Go to Average Wait State
    {
        Current_state_SM2 = stateTable[1].CURRENT_STATE;
    }
    return 0;
}


int Handle_Average_Wait()
{

    int8_t data[4];                                         //buffer to store data for average
    int8_t current_data;


    data[timeout_val]=i2c_read_bytes_avg_alert(addr,temp_addr);    //put the data into an array to perform average
    current_data=data[timeout_val];

    if (data[timeout_val]==1)
    {
        return 1;
    }


    if (AVERAGE==0)
    {
        AVERAGE=data[timeout_val];                             //if first reading, avg is same as data
    }
    else if (AVERAGE!=0)
    {
        AVERAGE=(AVERAGE+data[timeout_val])/2;				//avg is last reading plus current reading by 2
    }

    log_Handle_Average_Wait(current_data,AVERAGE);           //print msg for average

    if (nack_f == 1)    								  //Disconnected
    {
        Current_state_SM2 = stateTable[3].CURRENT_STATE;
        log_next_line();

        if (ut_f==1)										//check for the unit test flag
        {
            log_next_line();

            UCUNIT_CheckIsEqual(nack_f,1); 					 //perform unit test
            log_next_line();

        }
        log_next_line();

    }
    else													//Temperature read operation
    {
        Current_state_SM2 = stateTable[0].CURRENT_STATE;   //change state to temp read
        timeout_val++;

        delay(timeout_delay);
    }
    return 0;
}

int Handle_Temperature_Alert()
{


    int alert_check=i2c_read_bytes_avg_alert(addr,temp_addr);     //read temperature data for alert checkinh
    log_alert(alert_check);
    if (alert_check==1)
    {
        return 1;
    }

    int alert_check1=i2c_read_bytes(addr,temp_addr,1);
    if(alert_check1 ==128 || alert_check1 ==-128  )               //checking alert condition
    {

        al_f =1;
        Current_state_SM2 = stateTable[3].CURRENT_STATE;
        if (db_f == 1)
        {
            blink(2,1000);
        }                                          //blue led blink
    }
    if (alert_check1==5)
    {
        Current_state_SM2 = stateTable[1].CURRENT_STATE;
        return 5;

    }
    //RESET the alert Flag
    if (nack_f == 1)     									 //Disconnected
    {
        Current_state_SM2 = stateTable[3].CURRENT_STATE;
        log_next_line();

        if (ut_f==1)												//check for the unit test flag
        {
            log_next_line();

            UCUNIT_CheckIsEqual(nack_f,1); 					//unit test
            log_next_line();

        }
        log_next_line();

    }
    else
    {
        al_f =0;
        Current_state_SM2 = stateTable[1].CURRENT_STATE;
    }
    return 0;
}

int Handle_Disconnect()
{

    if (db_f == 1)                    //check for debug flag to glow the leds
    {
        blink(0,10000);               //blink the red led for disconnected state
    }


    return 1;


}

int POST()
{
    log_POST(1);
    int post_disc=i2c_read_bytes_post(addr, temp_addr, 2);   //reading data for post

    if (post_disc==1)									//checking if post is successful
    {
        return 1;
    }

    if (nack_f == 0)
    {
        {
            if (db_f == 1)     						 //check for debug flag to glow the leds
            {
                blink(0,250);   				//for post operation blink group of rgb
                //leds to signify initialization
                blink(1,250);
                blink(2,250);
            }
        }
        log_POST(2);

    }
    else
    {
        int disconnect7= Handle_Disconnect();  //go to disconnect function
        if (disconnect7==1)
            return 1;
    }
    PRINTF("\n\rTemperature values=");
    i2c_read_bytes(addr, temp_addr,0);


    wait_to_complete();

    i2c_write_byte(addr,temp_high_register,Temp_high_byte1,Temp_high_byte2);    //write temp high register values into tmp102

    wait_to_complete();

    PRINTF("\n\rTEMP_HIGH_VALUES\n\r");
    i2c_read_bytes(addr, temp_high_register,0);                 //read temp high register values into tmp102

    wait_to_complete();

    i2c_write_byte(addr,temp_low_register,Temp_low_byte1,Temp_low_byte2);  //write temp low register values into tmp102

    wait_to_complete();

    PRINTF("\n\rTEMP_LOW_VALUES\n\r");
    i2c_read_bytes(addr, temp_low_register,0);

    wait_to_complete();

    i2c_write_byte(addr,config_addr,Config_byte1,Config_byte2);    	//write temp high register value into tmp102
    wait_to_complete();

    log_blink_led(1);
    return 0;

}

int state_machines()
{

    int post_value=POST();
    if (post_value==1)
    {
        return 1;
    }


    while(1)
    {
        ////////////////////////////////////////////////////////////////////////////////////////



        //////////////////////////////State Machine 1//////////////////////////////////////////

        PRINTF("\n\r State Machine 1\n\r");

        //State Machine Initializations
        Current_state_SM1 = TEMP_READ;     //Default starting state

        sm_f = 0;                        //Set State Machine to 1
        timeout_val = 0;				  //Reset Timeout value to 1
        while(timeout_val < 4 )
        {
            switch(Current_state_SM1)
            {
            case TEMP_READ:
            {
                log_state_machines(1);
                int st_tempread=Handle_Temp_Read();  //read the temperature in the temp read state
                if (st_tempread==1)
                {
                    return 1;
                }
                //    Read temperature here and check flags,performing required operation
                if (db_f == 1)
                {
                    blink(1,1000);
                }
                if (nack_f == 1)    				  //Disconnected state flag check
                {
                    Current_state_SM1 = DISCONNECT;
                    log_next_line();

                    if (ut_f==1)                   //check for the unit test flag
                    {
                        log_next_line();

                        UCUNIT_CheckIsEqual(nack_f,1);   //Unit testing
                        log_next_line();

                    }
                    log_next_line();

                    break;
                }
                if (st_tempread==5)        //Temperature Alert
                {
                    Current_state_SM1 = TEMP_ALERT;   //the transition will be to Temperature Alert


                    log_next_line();

                    if (ut_f==1)				//check for the unit test flag
                    {
                        log_next_line();

                        UCUNIT_CheckIsEqual(st_tempread,1);  //Unit testing
                        log_next_line();

                    }
                    log_next_line();

                    break;
                }
                else                  //Normal Temperature read
                {
                    log_next_line();

                    if (ut_f==1)
                    {
                        log_next_line();

                        UCUNIT_CheckIsEqual(Current_state_SM1,TEMP_READ);   //Unit testing
                        log_next_line();

                    }
                    log_next_line();

                    Current_state_SM1 = AVERAGE_WAIT;      // go to average/wait state
                    break;
                }
                break;
            }

            case AVERAGE_WAIT:
            {


                if (db_f == 1)
                {
                    blink(1,1000);							//blink the green led
                }
                log_state_machines(2);                     //log state machine

                int st_average_wait=Handle_Average_Wait();     //read the temperature value to perform average
                if (st_average_wait==1)
                {
                    return 1;
                }
                if (nack_f == 1)     						 //Disconnected
                {
                    Current_state_SM1 = DISCONNECT;
                    log_next_line();
                    //the transition will be to Disconnected
                    if (ut_f==1)
                    {
                        log_next_line();

                        UCUNIT_CheckIsEqual(nack_f,1);      //unit test
                        log_next_line();

                    }
                    log_next_line();

                    break;
                }

                else
                {
                    log_next_line();
                    									//else go into the average/wait state
                    if (ut_f==1)      					//check for the unit test flag
                    {
                        log_next_line();

                        UCUNIT_CheckIsEqual(Current_state_SM1,AVERAGE_WAIT);   //unit test
                        log_next_line();

                    }
                    log_next_line();

                    Current_state_SM1 = TEMP_READ;    //the transition will be to temp read


                    delay(timeout_delay);
                    break;
                }
                break;
            }
            case TEMP_ALERT:
            {

                if (db_f == 1)                               //check for debug flag
                {
                    blink(2,1000);
                }                                          //blue led blink
                log_state_machines(3);

                int st_temp_alert=Handle_Temperature_Alert();  //perform read for detecting alert state
                if (st_temp_alert==1)
                {
                    return 1;
                }
                al_f =0;     	    						  //RESET the alert Flag
                if (nack_f == 1)  							    //Disconnected
                {
                    Current_state_SM1 = DISCONNECT;      //the transition will be to disconnected
                    if (ut_f==1)
                    {
                        log_next_line();					//log for next line

                        UCUNIT_CheckIsEqual(nack_f,1);   //carry out unit test
                        log_next_line();

                    }
                    break;
                }
                else
                {
                    Current_state_SM1 = AVERAGE_WAIT;    //the transition will be to average wait
                    break;
                }
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
            {
                if (db_f == 1)
                {
                    blink(0,1000);
                }
                int disconnect2= Handle_Disconnect();  //go to disconnect function
                if (disconnect2==1)
                    return 1;
                break;
            }


            }

        }
////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////State Machine 2//////////////////////////////////////////
        timeout_val = 0;
        Current_state_SM2 = TEMP_READ;       //initializing first state
        log_state_machines(4);
//        PRINTF("\n \n\r State Machine 2\n\r");


        while (timeout_val < 4 )                    //
        {
            if (Current_state_SM2 == stateTable[0].CURRENT_STATE)
            {
                log_state_machines(5);
                if (db_f == 1)           //check for the debug status and blink led
                {
                    blink(1,1000);
                }

                int st_temp=Handle_Temp_Read();      //read temperature in temp read state
                if (st_temp==1)
                {
                    return 1;
                }
                else if (st_temp==5)        //Temperature Alert
                {
                    log_next_line();

                    if (ut_f==1)				//check for the unit test flag
                    {
                        log_next_line();

                        UCUNIT_CheckIsEqual(st_temp,1);        //checking unit test
                        log_next_line();

                    }
                    log_next_line();



                }

                if (nack_f==1)            //check for no ack in the read function and got to disconnect
                {
                    int disconnect3= Handle_Disconnect();  //go to disconnect function
                    if (disconnect3==1)
                        return 1;

                }
            }
            if (Current_state_SM2 == stateTable[1].CURRENT_STATE)
            {
                log_state_machines(6);

                int st_wait=Handle_Average_Wait();  //read temperature to perform average operation
                if (st_wait==1)
                {
                    return 1;
                }
                if (db_f == 1)         //check for the debug status and blink led
                {
                    blink(1,1000);
                }
                if (nack_f==1)      //check for no ack in the read function and go to disconnect
                {
                    int disconnect4= Handle_Disconnect();  //go to disconnect function
                    if (disconnect4==1)
                        return 1;
                }

            }

            if (Current_state_SM2 == stateTable[2].CURRENT_STATE)
            {

                if (db_f == 1)      //check for the debug status and blink led
                {
                    blink(2,1000);
                }

                int st_alert=Handle_Temperature_Alert();   //read temperature to see for alert
                if (st_alert==1)
                {
                    return 1;
                }
                else if (st_alert==5)        //Temperature Alert
                {
                    log_Handle_Temperature_Alert();
                    log_next_line();

                    if (ut_f==1)				//check for the unit test flag
                    {
                        log_next_line();

                        UCUNIT_CheckIsEqual(st_alert,1);        //checking unit test
                        log_next_line();

                    }
                    log_next_line();
                }


                if (nack_f==1)    //check for no ack in the read function and go to disconnect
                {
                    int disconnect5= Handle_Disconnect();  //go to disconnect function
                    if (disconnect5==1)
                        return 1;

                }
            }
            if (Current_state_SM2 == stateTable[3].CURRENT_STATE)
            {

                int disconnect6= Handle_Disconnect();  //go to disconnect function
                if (disconnect6==1)
                    return 1;  //go to disconnect function
            }

        }

    }

}
