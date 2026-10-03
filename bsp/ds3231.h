/*
 * ds3231.h
 *
 *  Created on: Oct 1, 2026
 *      Author: EE-11
 */
#include "stm32f407xx.h"

#ifndef DS3231_H_
#define DS3231_H_

#define DS3231_ADDR_SEC		0x00
#define DS3231_ADDR_MIN		0x01
#define DS3231_ADDR_HOUR	0x02
#define DS3231_ADDR_DAY		0x03
#define DS3231_ADDR_DATE	0x04
#define DS3231_ADDR_MONTH	0x05
#define DS3231_ADDR_YEAR	0x06

#define DS3231_ADDR_CR		0x0E
#define DS3231_ADDR_ST		0x0F

#define DS3231_24HR_FORMAT	0
#define DS3231_12HR_FORMAT	1

#define DS3231_12HR_AM		0
#define DS3231_12HR_PM		1

#define DS3231_I2C_ADDR		0x68

#define MONDAY				1
#define TUESDAY				2
#define WEDNESDAY			3
#define THURSDAY			4
#define FRIDAY				5
#define SATURDAY			6
#define SUNDAY				7

/*
 * I2C config
 */
#define DS3231_I2C						I2C1
#define DS3231_I2C_GPIO_PORT			GPIOB
#define DS3231_I2C_GPIO_PIN_SCL			GPIO_PIN_NO_6
#define DS3231_I2C_GPIO_PIN_SDA			GPIO_PIN_NO_7
#define DS3231_I2C_SPEED				I2C_SCL_SPEED_SM
#define DS1307_I2C_PUPD					GPIO_NO_PUPD

typedef struct
{
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t day;
}RTC_date_t;


typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t time_format;
}RTC_time_t;


void ds3231_i2c_config_init(void);
void ds3231_i2c_gpio_init(void);

void DS3231_set_current_time(RTC_time_t *);
void DS3231_get_current_time(RTC_time_t *);

void DS3231_set_current_date(RTC_date_t *);
void DS3231_get_current_date(RTC_date_t *);


#endif /* DS3231_H_ */
