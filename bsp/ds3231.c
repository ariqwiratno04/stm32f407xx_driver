/*
 * ds3231.c
 *
 *  Created on: Oct 1, 2026
 *      Author: EE-11
 */

#include "ds3231.h"

static uint8_t bcd_to_binary(uint8_t value);
static uint8_t binary_to_bcd(uint8_t value);

I2C_Handle_t ds3231handle;

void I2C_ds3231_init(void)
{
	ds3231handle.I2C_Config.I2C_DeviceAddress 	= DS3231_I2C_ADDR;
	ds3231handle.I2C_Config.I2C_ACKControl		= I2C_ACK_ENABLE;
	ds3231handle.I2C_Config.I2C_SCLSpeed		= I2C_SCL_SPEED_SM;
}

static uint8_t bcd_to_binary(uint8_t value)
{
	uint8_t m , n;
	m = (uint8_t) ((value >> 4 ) * 10);
	n =  value & (uint8_t)0x0F;
	return (m+n);
}

static uint8_t binary_to_bcd(uint8_t value)
{
	uint8_t m , n;
	uint8_t bcd;

	bcd = value;
	if(value >= 10)
	{
		m = value /10;
		n = value % 10;
		bcd = (m << 4) | n ;
	}

	return bcd;
}
