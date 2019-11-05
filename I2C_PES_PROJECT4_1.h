#ifndef I2C_PES_PROJECT4_1_H_
#define I2C_PES_PROJECT4_1_H_

#include <stdio.h>

#include <stdint.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_common.h"
#include "MKL25Z4.h"
#include <fsl_debug_console.h>


#define I2C_M_START   I2C0->C1 |= I2C_C1_MST_MASK      //start condition
#define I2C_M_STOP    I2C0->C1 &= ~I2C_C1_MST_MASK		//master mode enable condition
#define I2C_M_RSTART  I2C0->C1 |= I2C_C1_RSTA_MASK		//restart condition

#define I2C_TRAN      I2C0->C1 |= I2C_C1_TX_MASK      //transmit set mask
#define I2C_REC       I2C0->C1 &= ~I2C_C1_TX_MASK    //receive set mask

#define I2C_WAIT      while((I2C0->S & I2C_S_IICIF_MASK)==0) {} \
                       I2C0->S |= I2C_S_IICIF_MASK;         //wait condition for read waiting for interrupt


#define NACK  I2C0->C1 |= I2C_C1_TXAK_MASK  //No  Acknowledgement after last write byte
#define ACK   I2C0->C1 &= ~I2C_C1_TXAK_MASK  // Acknowledgment after last write byte

#define addr  0x90
#define config_addr  0x01
#define temp_high_register  0x03
#define temp_low_register  0x02

#define temp_addr  0x00
#define Temp_high_byte1 0x25
#define Temp_high_byte2 0x00

#define Temp_low_byte1 0x17
#define Temp_low_byte2 0x00



#define Config_byte1 0x6C
#define Config_byte2 0xA0

//#define MASK(x)    (1UL << (x))
//#define RED_LED    18


void i2c_init(void);
void i2c_write_byte(uint8_t dev, uint8_t reg, uint8_t data_byte1,uint8_t data_byte2);
int i2c_read_bytes(uint8_t dev_adx,uint8_t reg_adx,int CR);
void i2c_read_byte(uint8_t dev, uint8_t reg);

#endif
