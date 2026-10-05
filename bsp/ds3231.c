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
	uint8_t control;

	ds3231_i2c_config_init();
	ds3231_i2c_gpio_init();

	I2C_PeripheralControl(I2C1, ENABLE);

	// Read control register and clear only EOSC
	control = ds3231_read(DS3231_ADDR_CR);
	control &= ~(1U << 7);
	ds3231_write(control, DS3231_ADDR_CR);

	// Read back EOSC: 0 = cleared, 1 = still set
	control = ds3231_read(DS3231_ADDR_CR);

	return (control >> 7) & 0x01U;

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
	ds3231_i2c_scl.GPIO_PinConfig.GPIO_PinPuPdControl = DS3231_I2C_PUPD;
	ds3231_i2c_scl.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

	GPIO_Init(&ds3231_i2c_scl);


	ds3231_i2c_sda.pGPIOx = DS3231_I2C_GPIO_PORT;
	ds3231_i2c_sda.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_AF4;
	ds3231_i2c_sda.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALT;
	ds3231_i2c_sda.GPIO_PinConfig.GPIO_PinNumber = DS3231_I2C_GPIO_PIN_SDA;
	ds3231_i2c_sda.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	ds3231_i2c_sda.GPIO_PinConfig.GPIO_PinPuPdControl = DS3231_I2C_PUPD;
	ds3231_i2c_sda.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

	GPIO_Init(&ds3231_i2c_sda);
}

void ds3231_set_current_time(RTC_time_t *rtc_time)
{
	uint8_t hrs;

	//write seconds using bcd
	ds3231_write(binary_to_bcd(rtc_time->seconds), DS3231_ADDR_SEC);
	//write minutes using bcd
	ds3231_write(binary_to_bcd(rtc_time->minutes), DS3231_ADDR_MIN);

	hrs = binary_to_bcd(rtc_time->hours);

	//check the AM/PM format
	if(rtc_time->time_format == TIME_FORMAT_24HRS){
		hrs &= ~(1 << 6);
	}else{
		hrs |= (1 << 6);
		hrs = (rtc_time->time_format  == TIME_FORMAT_12HRS_PM) ? hrs | ( 1 << 5) :  hrs & ~( 1 << 5) ;
	}
	//write hours
	ds3231_write(hrs,DS3231_ADDR_HOUR);
}

void ds3231_get_current_time(RTC_time_t *rtc_time)
{
	uint8_t hrs;

	rtc_time->seconds = bcd_to_binary(ds3231_read(DS3231_ADDR_SEC));

	rtc_time->minutes = bcd_to_binary(ds3231_read(DS3231_ADDR_MIN));

	hrs = ds3231_read(DS3231_ADDR_HOUR);

		if(hrs & ( 1 << 6)){
			//12 hr format
			rtc_time->time_format = (hrs & (1 << 5)) ? TIME_FORMAT_12HRS_PM : TIME_FORMAT_12HRS_AM;
			hrs &= ~(0x3 << 5);//Clear 6 and 5
		}else{
			//24 hr format
			rtc_time->time_format = TIME_FORMAT_24HRS;
		}

		rtc_time->hours = bcd_to_binary(hrs);
}

void ds3231_set_current_date(RTC_date_t *rtc_date)
{
	//write the date
	ds3231_write(binary_to_bcd(rtc_date->date), DS3231_ADDR_DATE);
	//write the month
	ds3231_write(binary_to_bcd(rtc_date->month), DS3231_ADDR_MONTH);
	//write the year
	ds3231_write(binary_to_bcd(rtc_date->year), DS3231_ADDR_YEAR);
	//write the day
	ds3231_write(binary_to_bcd(rtc_date->day), DS3231_ADDR_DAY);
}

void ds3231_get_current_date(RTC_date_t *rtc_date)
{
	uint8_t month;

	//get and store day and date value
	rtc_date->day = bcd_to_binary(ds3231_read(DS3231_ADDR_DAY));
	rtc_date->date = bcd_to_binary(ds3231_read(DS3231_ADDR_DATE));

	month = ds3231_read(DS3231_ADDR_MONTH);

	//read and store the century bit
	rtc_date->century = (month >> 7) & 0x01;
	//clear the century bit
	month  &= ~(1 << 7);

	//get and store the month
	rtc_date->month = bcd_to_binary(month);
	//get and store the year
	rtc_date->year = bcd_to_binary(ds3231_read(DS3231_ADDR_YEAR));

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
	I2C_MasterSendData(&ds3231handle, &reg_addr, 1, DS3231_I2C_ADDR, 1);
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
