/*
 * ds3231_test.c
 *
 *  Created on: Oct 5, 2026
 *      Author: EE-11
 */

/*
 * PB6 --> I2C1_SCL
 * PB7 --> I2C1_SDA
 */

#include "stm32f407xx.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "ds3231.h"

void Button_Inits(void){

	GPIO_Handle_t GpioButton;

	GpioButton.pGPIOx 								= GPIOA;
	GpioButton.GPIO_PinConfig.GPIO_PinNumber 		= GPIO_PIN_NO_0;
	GpioButton.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_IN;
	GpioButton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_SPEED_HIGH;
	GpioButton.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_NO_PUPD;

	GPIO_Init(&GpioButton);
}

void delay(void)
{
	for(uint32_t i = 0; i < 500000; i++);
}

char* get_day_of_week(uint8_t i)
{
	char* days[] = { "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday", "Sunday"};

	return days[i-1];
}


void number_to_string(uint8_t num , char* buf)
{

	if(num < 10){
		buf[0] = '0';
		buf[1] = num + 48; //convert to ASCII value
	}else if(num >= 10 && num < 99)
	{
		buf[0] = (num/10) + 48; //convert to ASCII value
		buf[1]= (num % 10) + 48; //convert to ASCII value
	}
}

char* time_to_string(RTC_time_t *rtc_time)
{
	//hh:mm:ss
	static char buf[9];

	buf[2]= ':';
	buf[5]= ':';

	number_to_string(rtc_time->hours,buf);
	number_to_string(rtc_time->minutes,&buf[3]);
	number_to_string(rtc_time->seconds,&buf[6]);

	buf[8] = '\0';

	return buf;

}

char* date_to_string(RTC_date_t *rtc_date)
{
	//dd/mm/y
	static char buf[9];

	buf[2]= '/';
	buf[5]= '/';

	number_to_string(rtc_date->date,buf);
	number_to_string(rtc_date->month,&buf[3]);
	number_to_string(rtc_date->year,&buf[6]);

	buf[8]= '\0';

	return buf;

}


int main(void)
{
	Button_Inits();

	RTC_time_t current_time;
	RTC_date_t current_date;

	printf("RTC test \n");

	if(ds3231_init())
	{
		printf("Init failed\n");
		while(1);
	}


	current_date.day = MONDAY;
	current_date.date = 5;
	current_date.month = 10;
	current_date.year = 26;

	current_time.hours = 4;
	current_time.minutes = 42;
	current_time.seconds = 0;
	current_time.time_format = TIME_FORMAT_12HRS_PM;

	ds3231_set_current_date(&current_date);
	ds3231_set_current_time(&current_time);

	while(1){
	//Wait button press
	while(! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));
	delay();
	printf("Button pressed\n");

	ds3231_get_current_time(&current_time);
	ds3231_get_current_date(&current_date);

	char *am_pm;
		if(current_time.time_format != TIME_FORMAT_24HRS){
			am_pm = (current_time.time_format) ? "PM" : "AM";
			printf("Current time = %s %s\n",time_to_string(&current_time),am_pm); // 04:25:41 PM
		}else{
			printf("Current time = %s\n",time_to_string(&current_time)); // 04:25:41
		}

	printf("Current date = %s <%s>\n",date_to_string(&current_date), get_day_of_week(current_date.day));
	}

	return 0;
}
