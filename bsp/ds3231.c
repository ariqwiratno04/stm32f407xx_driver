/*
 * ds3231.c
 *
 *  Created on: Oct 1, 2026
 *      Author: EE-11
 */

#include "ds3231.h"
#include <string.h>
#include <stdint.h>

static void ds3231_write(uint8_t value, uint8_t reg_addr);
static uint8_t ds3231_read(uint8_t reg_addr);

static uint8_t bcd_to_binary(uint8_t value);
static uint8_t binary_to_bcd(uint8_t value);

I2C_Handle_t ds3231handle;

uint8_t ds3231_init(void)
{
	ds3231_i2c_config_init();
	ds3231_i2c_gpio_init();

	I2C_PeriClockControl(DS3231_I2C, ENABLE);

	//Enable the DS3231 clock oscillator to 0 on bit 7 ^EOSC (0xE)
	ds3231_write(0x00, DS3231_ADDR_CR);

	//Read back EOSC bit
	uint8_t clock_state = ds3231_read(DS3231_ADDR_ST);

	return ((clock_state >> 7 ) & 0x1);

}

void ds3231_i2c_config_init(void)
{
	ds3231handle.pI2Cx							= DS3231_I2C;
	ds3231handle.I2C_Config.I2C_ACKControl		= I2C_ACK_ENABLE;
	ds3231handle.I2C_Config.I2C_SCLSpeed		= DS3231_I2C_SPEED;

	I2C_Init(&ds3231handle);
}

void ds3231_i2c_gpio_init(void)
{
	GPIO_Handle_t ds3231_i2c_scl, ds3231_i2c_sda;

	memset(&ds3231_i2c_scl,0,sizeof(ds3231_i2c_scl));
	memset(&ds3231_i2c_sda,0,sizeof(ds3231_i2c_sda));

	ds3231_i2c_scl.pGPIOx = DS3231_I2C_GPIO_PORT;
	ds3231_i2c_scl.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_AF4;
	ds3231_i2c_scl.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALT;
	ds3231_i2c_scl.GPIO_PinConfig.GPIO_PinNumber = DS3231_I2C_GPIO_PIN_SCL;
	ds3231_i2c_scl.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	ds3231_i2c_scl.GPIO_PinConfig.GPIO_PinPuPdControl = DS1307_I2C_PUPD;
	ds3231_i2c_scl.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

	GPIO_Init(&ds3231_i2c_scl);


	ds3231_i2c_sda.pGPIOx = DS3231_I2C_GPIO_PORT;
	ds3231_i2c_sda.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_AF4;
	ds3231_i2c_sda.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALT;
	ds3231_i2c_sda.GPIO_PinConfig.GPIO_PinNumber = DS3231_I2C_GPIO_PIN_SDA;
	ds3231_i2c_sda.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	ds3231_i2c_sda.GPIO_PinConfig.GPIO_PinPuPdControl = DS1307_I2C_PUPD;
	ds3231_i2c_sda.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

	GPIO_Init(&ds3231_i2c_sda);
}

static void ds3231_write(uint8_t value, uint8_t reg_addr)
{
	uint8_t tx[2];
	tx[0] = reg_addr;
	tx[1] = value;
	I2C_MasterSendData(&ds3231handle, tx, 2, DS3231_I2C_ADDR, 0);

}

static uint8_t ds3231_read(uint8_t reg_addr)
{
	uint8_t data;
	I2C_MasterSendData(&ds3231handle, &reg_addr, 1, DS3231_I2C_ADDR, 0);
	I2C_MasterReceiveData(&ds3231handle, &data, 1, DS3231_I2C_ADDR , 0);

	return data;
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
