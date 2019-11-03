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
#include <stdio.h>
//#include <stdio.h>
//#include <stdlib.h>
#include <stdint.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
//#include "i2c.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */


#define I2C_M_START   I2C0->C1 |= I2C_C1_MST_MASK
#define I2C_M_STOP    I2C0->C1 &= ~I2C_C1_MST_MASK
#define I2C_M_RSTART  I2C0->C1 |= I2C_C1_RSTA_MASK

#define I2C_TRAN      I2C0->C1 |= I2C_C1_TX_MASK;
#define I2C_REC       I2C0->C1 &= ~I2C_C1_TX_MASK;

#define I2C_WAIT      while((I2C0->S & I2C_S_IICIF_MASK)==0) {} \
					I2C0->S |= I2C_S_IICIF_MASK;


#define NACK  I2C0->C1 |= I2C_C1_TXAK_MASK
#define ACK   I2C0->C1 &= ~I2C_C1_TXAK_MASK
#define ALERT I2C0->SMB= I2C0->SMB |= I2C_SMB_ALERTEN_MASK

void i2c_init(void)
{
	//clock i2c peripheral and port c
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

	//set pins to i2c function
	PORTC ->PCR[8] |= PORT_PCR_MUX(2);
	PORTC->PCR[9] |= PORT_PCR_MUX(2);



	PORTC->PCR[16] |=  PORT_PCR_MUX(1);
	PORTC->PCR[16] |= PORT_PCR_PS_MASK;
	FGPIO_PDDR_PDD(16);

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

void i2c_write_byte(uint8_t dev, uint8_t reg, uint8_t data)
{
        I2C_TRAN;          /*SET TO TRANSMIT MODE*/
        I2C_M_START;       /*SEND START*/
        I2C0->D = dev;     /*send dev address (write)*/
        I2C_WAIT;           /*WAIT FOR ACK*/

        I2C0->D = reg;     /*send register address*/
        I2C_WAIT;

        I2C0->D = data;    /*send data*/
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
        PRINTF("\n\r%d",data_buf[0]);
        PRINTF("\n\r%d",data_buf[1]);

        return 1;
}




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
	uint8_t data[2];
	uint8_t addr = 0x90;

	uint8_t temp_addr = 0x00;
	i2c_read_bytes(addr, temp_addr);


	uint8_t config_addr=0x01;
	uint8_t temp_high_register=0x03;


	i2c_write_byte(addr,config_addr,0x0C);
	i2c_write_byte(addr,config_addr,0xA0);
	i2c_write_byte(addr,temp_high_register,0x1E);
	i2c_write_byte(addr,temp_high_register,0x00);

	ALERT;


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
	volatile static int i = 0 ;
	/* Enter an infinite loop, just incrementing a counter. */
	while(1) {
		i2c_read_bytes(addr, temp_addr);
		//i2c_read_byte(0x90, 0x00);
		//i2c_read_bytes();
		i++ ;
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
