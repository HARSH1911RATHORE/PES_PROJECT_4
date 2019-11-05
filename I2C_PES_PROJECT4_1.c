


#include "I2C_PES_PROJECT4_1.h"
#include "uCUnit.h"




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

struct sStateTableEntry stateTable[]=
{
    {TEMP_READ, AVERAGE_WAIT, TEMP_ALERT, DISCONNECT, dont_care, dont_care},    //Temp Read
    {AVERAGE_WAIT, dont_care, dont_care, DISCONNECT, TEMP_READ, dont_care},	//Average Wait
    {TEMP_ALERT, AVERAGE_WAIT, dont_care, DISCONNECT, dont_care,dont_care},	//Alert State
    {DISCONNECT, dont_care, dont_care, dont_care, dont_care, dont_care},	//Disconnect
};


static volatile int ut_f = 1;

static volatile int db_f = 0;

//#define ALERT I2C0->SMB= I2C0->SMB |= I2C_SMB_ALERTEN_MASK
//Global Flag Declarations
static volatile int counter2=0;
static volatile int l_f = 0;                    //Logger Flag , Set when logger is on or enable
static volatile int sm_f = 0;					//State Machine Flag    0 when State machine 1 , 1 when state machine 2
static volatile int al_f = 0;					//Alert flag            1 when temperature under threshold
static volatile int nack_f = 0;					//No_Acknowledgment Flag  default 0   , set to 1 when disconnected or no acknowledgment received
static volatile int timeout_val =1;             //value of timeout
int timeout_delay = 2000;     						//1 sec
int Prev_temp = 25;								//stores the last value of temerature to average
state_SM1 Current_state_SM1;
state_SM1 Current_state_SM2;

void i2c_init(void)
{
    //clock i2c peripheral and port c
    SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

    //set pins to i2c function and choose the pin to perform i2c
    //function as sda and sclk
    PORTC ->PCR[8] |= PORT_PCR_MUX(2);
    PORTC->PCR[9] |= PORT_PCR_MUX(2);


    PORTA->PCR[4] &=~ PORT_PCR_MUX_MASK;	//set port c pin 16 to gpio input
    PORTA->PCR[4] |=  PORT_PCR_MUX(1);
    PORTA->PCR[4] |= PORT_PCR_PE_MASK;  //pull up enable for alert pin
    PORTA->PCR[4] |= PORT_PCR_PS_MASK;

    PORTA->ISFR &=~PORT_ISFR_ISF(4);
    PORTA->PCR[4] |= PORT_PCR_IRQC(0x0B);
    PORTA->ISFR &=~PORT_ISFR_ISF(0x10);

    //PORTA->PCR[5] |= 	GPIO_PDDR_PDD(0);

    PTA->PDDR |= (0UL<<4);    //set the gpio pin to input
    //set to 400k baud
    //baud= bus freq/(scl_div+mul)
    //24MHz/400kHz = 60; icr=0x11 sets scl_div to 56
    I2C0->F = I2C_F_ICR(0x11) | I2C_F_MULT(0);

    //enable i2c and set to master mode
    I2C0->C1 |= (I2C_C1_IICEN_MASK);

    //select high drive mode
    I2C0->C2 |= (I2C_C2_HDRS_MASK);

    I2C0->SLTH = I2C_SLTL_SSLT(0x01);



}

void i2c_write_byte(uint8_t dev, uint8_t reg, uint8_t data_byte1,uint8_t data_byte2)
{
    I2C_TRAN;          /*SET TO TRANSMIT MODE*/
    I2C_M_START;       /*SEND START*/
    I2C0->D = dev;     /*send dev address (write)*/
    I2C_WAIT           /*WAIT FOR ACK*/

    I2C0->D = reg;     /*send register address*/
    I2C_WAIT

    I2C0->D = data_byte1;    /*send data byte 1*/
    I2C_WAIT

    I2C0->D = data_byte2;    /*send data byte 2*/
    I2C_WAIT

    I2C_M_STOP;
}








int i2c_read_bytes(uint8_t dev_adx,uint8_t reg_adx,int CR)
{

    uint8_t dummy;

    uint8_t data_buf[3];
    I2C_TRAN;             /*SET TO TRANSMIT MODE*/
    I2C_M_START;       /*SEND START*/
    I2C0->D = dev_adx;     /*send dev address (write)*/
    I2C_WAIT;           /*WAIT FOR completion*/
    if (nack_f==1)
    {
        return 0;
    }

    I2C0->D = reg_adx;     /*send register address (write)*/
    I2C_WAIT ;          /*WAIT FOR completion*/

    I2C_M_RSTART;       /*REPEATED START*/
    I2C0->D = dev_adx|0x01;     /*send dev address (read)*/

    I2C_WAIT  ;         /*WAIT FOR completion*/

    I2C_REC;           /*SET TO RECEIVE MODE*/
    ACK;              /*Tell hw to send ACK after read*/
    dummy = I2C0->D;    /*DUMMY READ TO START I2C READ*/
    I2C_WAIT   ;       /*WAIT FOR COMPLETION*/


    data_buf[0] = I2C0->D;  /*put value of temprature in data array*/
    I2C_WAIT;

    data_buf[1] = I2C0->D;
    I2C_WAIT;


    NACK;              /*Tell hw to send NACK after read*/

    I2C_M_STOP;         /*SEND STOP*/

    if (CR==0)

        PRINTF("\n\r%d",data_buf[0]);
    else if(CR==1)

        PRINTF("\n\r%d\n\r",data_buf[1]);

    return data_buf[0];
    // return data;
}



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
    i2c_read_bytes(addr,temp_addr,0);

    if (nack_f == 1)      //Disconnected
    {
        Current_state_SM2 = stateTable[3].CURRENT_STATE;
        PRINTF("\n\r");
        if (ut_f==1)
        {
            PRINTF("\n\r");
            UCUNIT_CheckIsEqual(nack_f,1);
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
//	int count average++;
    int i;
    int sum=0;
    int average;
    int data[5];
//	for (i=0;i<4;i++)

    data[timeout_val]=i2c_read_bytes(addr,temp_addr,0);



    for (i=0; i<=timeout_val; i++)
    {
        sum=sum+data[timeout_val];
        //     PRINTF("\n\rSUM=%d",sum);
    }
    average=sum/(timeout_val+1);
    PRINTF("\n\rCURRENT TEMPERATURE=%d\n\r",data[timeout_val]);
    PRINTF("\n\rAVERAGE TEMPERATURE=%d\n\r",average);


    if (nack_f == 1)      //Disconnected
    {
        Current_state_SM2 = stateTable[3].CURRENT_STATE;
        PRINTF("\n\r");
        if (ut_f==1)
        {
            PRINTF("\n\r");
            UCUNIT_CheckIsEqual(nack_f,1);
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
            UCUNIT_CheckIsEqual(nack_f,1);
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
    PRINTF("\n\n\r--------------------------------- Sensor Disconnected-----------------------\n\n\r ");
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
            if (db_f == 1)
            {
                blink(0,250);
                blink(1,250);
                blink(2,250);
            }
        }
        PRINTF("\n\n \n\r-------------------POST Successful----------------------------------------------\n\r");
    }
    else
    {
        PRINTF("\n \n\r The Temperature Sensor has been Disconnected\n\r");
    }




}

void wait_to_complete(void)
{
    volatile int l=100000;
    while(l!=0)
    {
        l--;
    }
}




int main(void)
{

    /* Init board hardware. */

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
    LED_RED_INIT(1);
    LED_GREEN_INIT(1);
    LED_BLUE_INIT(1);

    PRINTF("Hello World\n\r");
    i2c_init();

    POST();

    if(nack_f==1)
    {
        goto Disconnect_END;
    }




//    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
//
//    PORTB->PCR[18] &=~ PORT_PCR_MUX_MASK;
//    PORTB->PCR[18] |= PORT_PCR_MUX(1);
//





    PRINTF("\n\rTemperature values=");
    i2c_read_bytes(addr, temp_addr,0);


    wait_to_complete();



    i2c_write_byte(addr,temp_high_register,Temp_high_byte1,Temp_high_byte2);

    wait_to_complete();

    PRINTF("\n\rTEMP_HIGH_VALUES\n\r");
    i2c_read_bytes(addr, temp_high_register,0);

    wait_to_complete();


    i2c_write_byte(addr,temp_low_register,Temp_low_byte1,Temp_low_byte2);

    wait_to_complete();

    PRINTF("\n\rTEMP_LOW_VALUES\n\r");
    i2c_read_bytes(addr, temp_low_register,0);

    wait_to_complete();

    i2c_write_byte(addr,config_addr,Config_byte1,Config_byte2);
    wait_to_complete();


    PRINTF("\n\rALERT VALUES\n\r");     //checking for the alert state by reading the alert bit toggle

    i2c_read_bytes(addr, config_addr,1);   //storing the value
    //of alert bit in result








//
//    if (PTA->PDDR == (0UL << 4 ) )    //checking if port c pin connected to
//        //alert pin of tmp102 is an input pin
//    {
//        PTB->PDDR|=MASK(RED_LED);   //switching on the red led by
//        //setting the data direction register for red led
//        PTB->PCOR = MASK(RED_LED);
//    }












//    POST();


    /////////////////////////////////////changes here//////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////



    //////////////////////////state machine 1///////////////////////////////////////////////
    int counter =0;




State_Machine_1:                     //state machine 1 label
    PRINTF("\n\r State Machine 1\n\r");



    //State Machine Initializations
    Current_state_SM1 = TEMP_READ;     //Default starting state
    //Testing variable
    sm_f = 0;                        //Set State Machine to 1
    timeout_val = 0;				  //Reset Timeout value to 1
    while(timeout_val < 4)
    {

//        //State testing code
//        counter++;
//        if (counter ==15)
//        {
//            nack_f =1;
//        }
//        if (counter ==7)
//        {
//            al_f =1;
//        }

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
                if (ut_f==1)
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
                Current_state_SM1 = TEMP_ALERT;
                PRINTF("\n\r");
                if (ut_f==1)
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
                    PRINTF("\n\r");
                    UCUNIT_CheckIsEqual(Current_state_SM1,TEMP_READ);
                    PRINTF("\n\r");
                }
                PRINTF("\n\r");
                Current_state_SM1 = AVERAGE_WAIT;
                break;
            }

        case AVERAGE_WAIT:
            Handle_Average_Wait();
            if (db_f == 1)
            {
                blink(1,1000);
            }
            PRINTF("\n\r The sensor is in AVERAGE_WAIT STATE\n\r");
            if (nack_f == 1)      //Disconnected
            {
                Current_state_SM1 = DISCONNECT;
                PRINTF("\n\r");
                if (ut_f==1)
                {
                    PRINTF("\n\r");
                    UCUNIT_CheckIsEqual(nack_f,1);
                    PRINTF("\n\r");
                }
                PRINTF("\n\r");
                break;
            }
            else
            {
                PRINTF("\n\r");
                if (ut_f==1)
                {
                    PRINTF("\n\r");
                    UCUNIT_CheckIsEqual(Current_state_SM1,AVERAGE_WAIT);
                    PRINTF("\n\r");
                }
                PRINTF("\n\r");
                Current_state_SM1 = TEMP_READ;
                timeout_val++;
                delay(timeout_delay);
                break;
            }
        case TEMP_ALERT:
            Handle_Temperature_Alert();
            if (db_f == 1)
            {
                blink(2,1000);
            }
            PRINTF("\n\r The sensor is in TEMP_ALERT STATE\n\r");
            al_f =0;     	      //RESET the alert Flag
            if (nack_f == 1)      //Disconnected
            {
                Current_state_SM1 = DISCONNECT;
                if (ut_f==1)
                {
                    PRINTF("\n\r");
                    UCUNIT_CheckIsEqual(nack_f,1);
                    PRINTF("\n\r");
                }
                break;
            }
            else
            {
                Current_state_SM1 = AVERAGE_WAIT;
                break;
            }
        case DISCONNECT:
            Handle_Disconnect();
            if (db_f == 1)
            {
                blink(0,1000);
            }
            goto Disconnect_END;

        }

    }
    ////////////////////////////////////////////////////////////////////////////////////////



    //////////////////////////////State Machine 2//////////////////////////////////////////
    timeout_val = 0;
    Current_state_SM2 = TEMP_READ;       //initializing first state
    PRINTF("\n \n\r State Machine 2\n\r");
    //printf("\n%d",stateTable[0].goEvent);

    while (timeout_val < 4)
    {


//        //test codes to check working of the flags
//        counter++;
//        if (counter == 7)
//        {
//            al_f  = 1;
//        }



        if (Current_state_SM2 == stateTable[0].CURRENT_STATE)
        {
            PRINTF("\n \n\rWe are in Temperature read state\n\r");
            Handle_Temp_Read();
            if (db_f == 1)
            {
                blink(1,1000);
            }

        }
        if (Current_state_SM2 == stateTable[1].CURRENT_STATE)
        {
            PRINTF("\n\n\r We are in Temperature Wait Average state\n\r");
            Handle_Average_Wait();
            if (db_f == 1)
            {
                blink(1,1000);
            }

        }
        if (Current_state_SM2 == stateTable[2].CURRENT_STATE)
        {
            PRINTF("\n\r We are in Temperature Temperature Alert state\n\r");
            Handle_Temperature_Alert();
            if (db_f == 1)
            {
                blink(2,1000);
            }

        }
        if (Current_state_SM2 == stateTable[3].CURRENT_STATE)
        {
            PRINTF("\n\n\r We are in Disconnect state\n\r");
            Handle_Disconnect();
            if (db_f == 1)
            {
                blink(0,1000);
            }
            goto Disconnect_END;


        }

    }


    //go back to state machine 1
    goto State_Machine_1;











Disconnect_END:
    PRINTF("\n \n\r The Temperature Sensor has been Disconnected\n\r");

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


    __enable_irq();
    NVIC->ICPR[4] |= 1<<PORTA_IRQn ;
    NVIC->ISER[4] |= 1<<PORTA_IRQn ;
    return 0 ;
}

void PORTA_IRQHandler (void)
{
    PORTA->ISFR=PORT_ISFR_ISF(0x10);
    LED_BLUE_TOGGLE();
    PRINTF("entered  irq");
}



