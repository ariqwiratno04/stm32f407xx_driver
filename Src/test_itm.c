/*
 * test_itm.c
 *
 *  Created on: Sep 13, 2026
 *      Author: LEGION
 */
#include "stm32f407xx.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

extern void initialise_monitor_handles(void);

void delay(void)
{
	for(uint32_t i = 0; i < 500000; i++);
}

void Button_Inits(void){

	GPIO_Handle_t GpioButton;

	GpioButton.pGPIOx 								= GPIOA;
	GpioButton.GPIO_PinConfig.GPIO_PinNumber 		= GPIO_PIN_NO_0;
	GpioButton.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_IN;
	GpioButton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_SPEED_HIGH;
	GpioButton.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_NO_PUPD;

	GPIO_Init(&GpioButton);
}

int main(void){

	initialise_monitor_handles();

	Button_Inits();

	while(1){
		while(! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));
		delay();
		printf("Halo bang\n");
	}
}
