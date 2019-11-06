


#include "I2C_PES_PROJECT4_1.h"
#include "uCUnit.h"
#include "state_machine.h"
#include "system.h"


int noack_f=0;
static volatile int counter2=0;


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
    PTA->PDDR |= (0UL<<4);					//set the gpio pin to input
    PORTA->PCR[4] |=  PORT_PCR_MUX(1);
    PORTA->PCR[4] |= PORT_PCR_PE_MASK;  //pull up enable for alert pin
    PORTA->PCR[4] |= PORT_PCR_PS_MASK;

    //PORTA->ISFR &=~PORT_ISFR_ISF(4);
    PORTA->PCR[4] |= PORT_PCR_IRQC(0x0B);		//interrupt at either edge
    // PORTA->ISFR &=~PORT_ISFR_ISF(0x10);

    //PORTA->PCR[5] |= 	GPIO_PDDR_PDD(0);
    //set to 400k baud
    //baud= bus freq/(scl_div+mul)
    //24MHz/400kHz = 60; icr=0x11 sets scl_div to 56
    I2C0->F = I2C_F_ICR(0x11) | I2C_F_MULT(0);

    //enable i2c and set to master mode
    I2C0->C1 |= (I2C_C1_IICEN_MASK);

    //select high drive mode
    I2C0->C2 |= (I2C_C2_HDRS_MASK);
    //setting bit for timeout
    I2C0->SLTH = I2C_SLTL_SSLT(0x01);



}

void i2c_write_byte(uint8_t dev, uint8_t reg, uint8_t data_byte1,uint8_t data_byte2)
{
    I2C_TRAN;          /*SET TO TRANSMIT MODE*/
    I2C_M_START;       /*SEND START*/
    I2C0->D = dev;     /*send dev address (write)*/
    I2C_WAIT           /*WAIT FOR ACK*/

    I2C0->D = reg;     /*send register address*/
    I2C_WAIT			 /*WAIT FOR ACK*/

    I2C0->D = data_byte1;    /*send data byte 1*/
    I2C_WAIT				 /*WAIT FOR ACK*/

    I2C0->D = data_byte2;    /*send data byte 2*/
    I2C_WAIT				 /*WAIT FOR ACK*/

    I2C_M_STOP;				/*send a stop bit */

    log_i2c_write_bytes();
}








int i2c_read_bytes(uint8_t dev_adx,uint8_t reg_adx,int CR)
{
    uint8_t data_buf[3];  /*take an array of 3 elements to store the data values*/
    I2C_TRAN;             /*SET TO TRANSMIT MODE*/
    I2C_M_START;       /*SEND START*/
    I2C0->D = dev_adx;     /*send dev address (write)*/
    I2C_WAIT;           /*WAIT FOR completion*/

    I2C0->D = reg_adx;     /*send register address (write)*/
    I2C_WAIT ;          /*WAIT FOR completion*/

    I2C_M_RSTART;       /*REPEATED START*/
    I2C0->D = dev_adx|0x01;     /*send dev address (read)*/

    I2C_WAIT  ;         /*WAIT FOR completion*/

    I2C_REC;           /*SET TO RECEIVE MODE*/
    ACK;              /*Tell hw to send ACK after read*/
    data_buf[0] = I2C0->D;    /*DUMMY READ TO START I2C READ*/
    I2C_WAIT   ;       		/*WAIT FOR COMPLETION*/


    data_buf[0] = I2C0->D;  /*put value of temperature in data array*/
    I2C_WAIT;

    data_buf[1] = I2C0->D;    /*put data value byte 2 in data_buf[1]*/
    I2C_WAIT;

    NACK;              /*Tell hardware to send NACK after read*/

    I2C_M_STOP;         /*SEND STOP*/

    //log_i2c_read_bytes();


    if (CR==0)          /*check if it is a configuration byte read and print the value of
     	 	 	 	 	 	 	 	 	 desired byte*/
    //	log_i2c_read_bytes(0,data_buf[0]);
      PRINTF("\n\r%d",data_buf[0]);
    else if(CR==1)
    //	log_i2c_read_bytes(1,data_buf[1]);
        PRINTF("\n\r%d\n\r",data_buf[1]);

    if (data_buf[0]==144 && data_buf[0]==255)  //checking for disconnected state
    {
        nack_f=1;
        int disconnect8=Handle_Disconnect();   //going into the disconnected state
        if (disconnect8==1)
        {return 1;}


    }
    if (data_buf[1]==128)
    {
    	al_f =1;
    }
    return data_buf[0];

}

int i2c_read_bytes_post(uint8_t dev_adx,uint8_t reg_adx,int CR)
{
    uint8_t data_buf[3];  /*take an array of 3 elements to store the data values*/
    I2C_TRAN;             /*SET TO TRANSMIT MODE*/
    I2C_M_START;       /*SEND START*/
    I2C0->D = dev_adx;     /*send dev address (write)*/
    WAIT2;           /*WAIT FOR completion*/

if (noack_f==1)
{
	int disconnect9=Handle_Disconnect();
    if (disconnect9==1)
    {return 1;}

}

    I2C0->D = reg_adx;     /*send register address (write)*/
    WAIT2;          /*WAIT FOR completion*/

    I2C_M_RSTART;       /*REPEATED START*/
    I2C0->D = dev_adx|0x01;     /*send dev address (read)*/

    WAIT2  ;         /*WAIT FOR completion*/

    I2C_REC;           /*SET TO RECEIVE MODE*/
    ACK;              /*Tell hw to send ACK after read*/
    data_buf[0] = I2C0->D;    /*DUMMY READ TO START I2C READ*/
    if (data_buf[0]==0)
    {
    	int disconnect10=Handle_Disconnect();
        if (disconnect10==1)
        {return 1;}

    }
    WAIT2   ;       		/*WAIT FOR COMPLETION*/


    data_buf[0] = I2C0->D;  /*put value of temperature in data array*/
    WAIT2;

    data_buf[1] = I2C0->D;    /*put data value byte 2 in data_buf[1]*/
    WAIT2;

    NACK;              /*Tell hardware to send NACK after read*/

    I2C_M_STOP;         /*SEND STOP*/

    //log_i2c_read_bytes();


    if (CR==0)          /*check if it is a configuration byte read and print the value of
     	 	 	 	 	 	 	 	 	 desired byte*/
    //	log_i2c_read_bytes(0,data_buf[0]);
      PRINTF("\n\r%d",data_buf[0]);
    else if(CR==1)
    //	log_i2c_read_bytes(1,data_buf[1]);
        PRINTF("\n\r%d\n\r",data_buf[1]);

    return data_buf[0];

}


void wait_to_complete(void)
{
    volatile int l=100000;    //delay between successive write and read operation
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
    LED_GREEN_INIT(1);   //Initialization of rgb leds pin
    LED_BLUE_INIT(1);


    i2c_init();    //go to i2c init function





    //////////////////////////state machine 1///////////////////////////////////////////////


    int state_machine_test=state_machines();     //go to state machine logic

    if (state_machine_test==1)
    {PRINTF("\n\n\r--------------------------------- Sensor Disconnected-----------------------\n\n\r ");}


    __enable_irq();
    NVIC->ICPR[4] |= 1<<PORTA_IRQn ;  //enable irq for porta which works as a gpio and toggles the blue led
    NVIC->ISER[4] |= 1<<PORTA_IRQn ;
//    NVIC_EnableIRQ(IRQn)
    return 0 ;
}

void PORTA_IRQHandler (void)
{
    PORTA->ISFR=PORT_ISFR_ISF(0x10);
    LED_BLUE_TOGGLE();
    delay(1000);
    PRINTF("entered  irq");
}



