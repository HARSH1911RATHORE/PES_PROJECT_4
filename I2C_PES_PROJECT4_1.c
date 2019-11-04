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
 * @file    I2C_PES_PROJECT4_1.c
 * @brief   Application entry point.
 */

#include "I2C_PES_PROJECT4_1.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */



//#define ALERT I2C0->SMB= I2C0->SMB |= I2C_SMB_ALERTEN_MASK

void i2c_init(void)
{
	//clock i2c peripheral and port c
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

	//set pins to i2c function and choose the pin to perform i2c
	//function as sda and sclk
	PORTC ->PCR[8] |= PORT_PCR_MUX(2);
	PORTC->PCR[9] |= PORT_PCR_MUX(2);


	//
	PORTC->PCR[16] |=  PORT_PCR_MUX(1);
	PORTC->PCR[16] |= PORT_PCR_PE_MASK;
	PORTC->PCR[16] |= PORT_PCR_PS_MASK;

	PORTC->PCR[16] |= 	GPIO_PDDR_PDD(0);
	//	FGPIO_PDDR_PDD(16);

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
	I2C_WAIT;           /*WAIT FOR ACK*/

	I2C0->D = reg;     /*send register address*/
	I2C_WAIT;

	I2C0->D = data_byte1;    /*send data byte 1*/
	I2C_WAIT;

	I2C0->D = data_byte2;    /*send data byte 2*/
	I2C_WAIT;

	I2C_M_STOP;
}



//void i2c_read_byte(void)
//{
//	uint8_t data[3];
//
//	I2C_TRAN          /*SET TO TRANSMIT MODE*/
//	I2C_M_START;       /*SEND START*/
//	I2C0->D = 0x90;     /*send dev address (write)*/
//	I2C_WAIT           /*WAIT FOR completion*/
//
//	I2C0->D = 0x00;     /*send register address*/
//	I2C_WAIT           /*wait for completion*/
//
//	I2C_M_RSTART;       /*REPEATED START*/
//	I2C0->D = (0x90|0x01);     /*send dev address (read)*/
//	I2C_WAIT           /*WAIT FOR completion*/
//
//	I2C_REC           /*SET TO RECEIVE MODE*/
//	ACK;
//
//	/*SET TO NACK AFTER READ*/
//
//	data[0] = I2C0->D;    /*dummy read*/
//	I2C_WAIT           /*WAIT FOR COMPLETION*/
//	ACK;
//	//     I2C_M_STOP;        /*SEND STOP*/
//	data[0] = I2C0->D;     /*READ DATA*/
//	I2C_WAIT
//	ACK;
//	data[1] = I2C0->D;    /*dummy read*/
//	I2C_WAIT           /*WAIT FOR COMPLETION*/
//	ACK;
//	//     I2C_M_STOP;        /*SEND STOP*/
//	data[2] = I2C0->D;     /*READ DATA*/
//	I2C_WAIT
//	//   ACK;
//	//    data[2] = I2C0->D;    /*dummy read*/
//	//    I2C_WAIT           /*WAIT FOR COMPLETION*/
//	//    ACK;
//	//     I2C_M_STOP;        /*SEND STOP*/
//	//    data[2] = I2C0->D;     /*READ DATA*/
//	//    ACK;
//	NACK;
//	I2C_M_STOP;
//	PRINTF("%d",data[0]);
//	PRINTF("%d",data[1]);
//	PRINTF("%d",data[2]);
//	//   return data;
//}

#if 0
void i2c_read_byte(uint8_t dev, uint8_t reg)
{
	uint8_t dummy;
	uint8_t data;

	I2C_TRAN;          /*SET TO TRANSMIT MODE*/
	I2C_M_START;       /*SEND START*/
	I2C0->D = dev;     /*send dev address (write)*/
	I2C_WAIT           /*WAIT FOR completion*/
	// ACK;
	I2C0->D = reg;     /*send register address*/
	I2C_WAIT           /*wait for completion*/
	//	ACK;
	I2C_M_RSTART;       /*REPEATED START*/
	I2C0->D = (dev|0x1);     /*send dev address (read)*/
	I2C_WAIT           /*WAIT FOR completion*/
	//   ACK;
	I2C_REC;           /*SET TO RECEIVE MODE*/
	//  I2C_WAIT
	NACK;              /*SET TO NACK AFTER READ*/

	data = I2C0->D;    /*dummy read*/
	I2C_WAIT           /*WAIT FOR COMPLETION*/
	//ACK;
	I2C_M_STOP;        /*SEND STOP*/
	data = I2C0->D;     /*READ DATA*/
	//   I2C_WAIT
	// NACK;

	//        I2C_M_STOP;
	PRINTF("%d\n\r",data);

	// return data;
}

#endif
#if 0
void i2c_read_bytes(uint8_t dev_adx,uint8_t reg_adx,uint8_t * data, int8_t data_count)
{
	PRINTF("Step 1 Complete\n\r");
	uint8_t dummy = 0;
	uint8_t data[2];
	int i;
	int num_bytes_read=0;
	I2C_TRAN             /*SET TO TRANSMIT MODE*/
	I2C_M_START;       /*SEND START*/
	I2C0->D = 0x90;     /*send dev address (write)*/
	I2C_WAIT           /*WAIT FOR completion*/
	//ACK;

	I2C0->D = (uint8_t)0x00;     /*send register address (write)*/
	I2C_WAIT           /*WAIT FOR completion*/
	//ACK;
	I2C_M_RSTART;       /*REPEATED START*/
	I2C0->D = (uint8_t)0x90|0x1;     /*send dev address (read)*/
	I2C_WAIT           /*WAIT FOR completion*/
	//ACK;

	I2C_REC;           /*SET TO RECEIVE MODE*/
	ACK;             /*Tell hw to send ACK after read*/
	dummy=I2C0->D;    /*DUMMY READ TO START I2C READ*/

	PRINTF("%d\r\n",dummy);
	I2C_WAIT          /*WAIT FOR COMPLETION*/
	do
	{
		ACK;          /*Tell hw to send ACK after read*/
		data[num_bytes_read++] = I2C0->D; /*READ DATA*/
		I2C_WAIT       /*WAIT FOR COMPLETION*/
	}while (num_bytes_read < 2);

	NACK;              /*Tell hw to send NACK after read*/
	data[num_bytes_read++] = I2C0->D;  /*READ DATA*/
	I2C_WAIT            /*WAIT FOR COMPLETION*/
	I2C_M_STOP;         /*SEND STOP*/
	for (i=0;i<2;i++)
	{

		PRINTF("data = %d\r\n",data[i]);
	}
	//  return 1;
}

#endif

//int i2c_read_bytes(uint8_t dev_adx,uint8_t reg_adx,uint8_t * data, int8_t data_count)
int i2c_read_bytes(uint8_t dev_adx,uint8_t reg_adx)
{
	uint8_t dummy;
	//int8_t num_bytes_read=0;
	uint8_t data_buf[2];
	I2C_TRAN             /*SET TO TRANSMIT MODE*/
	I2C_M_START;       /*SEND START*/
	I2C0->D = dev_adx;     /*send dev address (write)*/
	I2C_WAIT           /*WAIT FOR completion*/
	I2C0->D = reg_adx;     /*send register address (write)*/
	I2C_WAIT           /*WAIT FOR completion*/
	I2C_M_RSTART;       /*REPEATED START*/
	//I2C0->D = dev_adx|0x1;     /*send dev address (read)*/
	I2C0->D = 0x91;
	I2C_WAIT           /*WAIT FOR completion*/

	I2C_REC;           /*SET TO RECEIVE MODE*/
	ACK;              /*Tell hw to send ACK after read*/
	dummy=I2C0->D;    /*DUMMY READ TO START I2C READ*/
	I2C_WAIT          /*WAIT FOR COMPLETION*/
	PRINTF("DUMMY=%d\n\r",dummy);

	/*  do
        {
            ACK;
            data[num_bytes_read++] = I2C0->D;
            I2C_WAIT
			PRINTF("%d\n\r",data[num_bytes_read]);
        }while (num_bytes_read < data_count-2);
	 */
	data_buf[0] = I2C0->D;
	I2C_WAIT

	data_buf[1] = I2C0->D;
	I2C_WAIT
	NACK;              /*Tell hw to send NACK after read*/
	//data[num_bytes_read++] = I2C0->D;  /*READ DATA*/
	//	PRINTF("%d\n\r",data[num_bytes_read]);
	I2C_M_STOP;         /*SEND STOP*/
	PRINTF("Temperature values=");
	PRINTF("\n\r%d",data_buf[0]);
	PRINTF("\n\r%d\n\r",data_buf[1]);

	return 1;
	// return data;
}

//int temp_reading(void)
//{
//	uint8_t dev_adx=0x90;
//	uint8_t reg_adx=0x00;
//	int current_temp=i2c_read_bytes(dev_adx,reg_adx);
//	return current_temp;
//}
//
//void average(void)
//{
//	int current_temperature=temp_reading();
//	for (i=0:i<n;i++)
//	{
//	int sum=sum+data[i];
//	}
//	int average_temperature=(sum)/n;
//	PRINTF("AVERAGE TEMPERATURE=%d\n\r",average_temperature);
//}
//
//void alert(void)
//{
//int alert=temp_reading;
//PRINTF("ALERT TEMPERATURE=%d\n\r",alert);
//}

//void disconnected(void)
//{
//
//}

int main(void) {

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
	//   int read[3];
	PRINTF("Hello World\n");
	i2c_init();

	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

	PORTB->PCR[18] &=~ PORT_PCR_MUX_MASK;
	PORTB->PCR[18] |= PORT_PCR_MUX(1);
	//	uint8_t data[2];
	//	uint8_t addr = 0x90;
	//	uint8_t temp_addr = 0x00;
	i2c_read_bytes(addr, temp_addr);

	volatile int l=100000;
	while(l!=0)
	{
		l--;
	}
	//uint8_t config_addr=0x01;
	//uint8_t temp_high_register=0x03;

	i2c_write_byte(addr,temp_high_register,Temp_high_byte1,Temp_high_byte2);

	volatile int j=100000;
	while(j!=0)
	{
		j--;
	}
	PRINTF("\n\rTEMP_HIGH_VALUES\n\r");
	i2c_read_bytes(addr, temp_high_register);

	volatile int m=100000;
		while(m!=0)
		{
			m--;
		}


	//	i2c_write_byte(addr,temp_high_register,Temp_high_byte2);
	//
	//	volatile int j=100000;
	//	while(j!=0)
	//	{
	//		j--;
	//	}

	i2c_write_byte(addr,temp_low_register,Temp_low_byte1,Temp_low_byte2);

	volatile int n=100000;
	while(n!=0)
	{
		n--;
	}
	PRINTF("\n\rTEMP_LOW_VALUES\n\r");
	i2c_read_bytes(addr, temp_low_register);

	volatile int k=100000;
	while(k!=0)
	{
		k--;
	}

	//	i2c_write_byte(addr,temp_low_register,Temp_low_byte1);
	//
	//	volatile int j=100000;
	//	while(j!=0)
	//	{
	//		j--;
	//	}

	i2c_write_byte(addr,config_addr,Config_byte1,Config_byte2);
	volatile int i=100000;
	while(i!=0)
	{
		i--;
	}
	//
	//	i2c_write_byte(addr,config_addr,Config_byte2);
	//	volatile int i=100000;
	//	while(i!=0)
	//	{
	//		i--;
	//	}

	int result=i2c_read_bytes(addr, config_addr);   //storing the value
	//of alert bit in result
	PRINTF("%d",result);
	//
	//	uint8_t config_addr=0x01;
	//
	//
	//	i2c_write_byte(addr,config_addr,0x0C);
	//	i2c_write_byte(addr,config_addr,0xA0);
	//	i2c_write_byte(addr,temp_high_register,0x1E);
	//	i2c_write_byte(addr,temp_high_register,0x00);
	//
	//	ALERT;




	if (PTC->PDDR == (0UL << 17 ) )    //checking if port c pin connected to
			//alert pin of tmp102 is an input pin
			{
		PTB->PDDR|=MASK(RED_LED);   //switching on the red led by
		//setting the data direction register for red led
		PTB->PCOR = MASK(RED_LED);
			}


	//i2c_read_bytes(addr, temp_addr);

	//i2c_write_byte(0x48,0x00,0x12);
	//   read[0]=i2c_read_byte(0x48,  0x00);
	//   PRINTF("%d",read[0]);
	//i2c_write_byte(0x48,0x00,0x14);
	//   read[1]=i2c_read_byte(0x48,  0x00);
	//   PRINTF("%d",read[1]);
	//i2c_write_byte(0x48,0x00,0x33);
	//  read[2]=i2c_read_byte(0x48,  0x00);
	//  PRINTF("%d",read[2]);
	/* Force the counter to be placed into memory. */
	volatile static int a = 0 ;
	/* Enter an infinite loop, just incrementing a counter. */
	while(1) {
		//	i2c_read_bytes(addr, temp_addr);
		//i2c_read_byte(0x90, 0x00);
		//i2c_read_bytes();
			a++ ;
		/* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
			__asm volatile ("nop");
	}

	//    __enable_irq();
	//    NVIC->ICPR[0] |= 1<<I2C0_IRQn;
	//    NVIC->ISER[0] |= 1<<I2C0_IRQn;
	return 0 ;
}

//void I2C0_IRQHandler (void)
//{
//	printf("entered  irq");
//}
