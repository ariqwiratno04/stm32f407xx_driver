/*
 * button_interrupt.c
 *
 *  Created on: Aug 2, 2026
 *      Author: LEGION
 */

#include "stm32f407xx.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

void delay(void)
{
	for(uint32_t i = 0; i < 500000; i++);
}


int main(void){

	GPIO_Handle_t GpioLedG, GpioLedO, GpioLedR, GpioBtn;

	memset(&GpioLedG, 0, sizeof(GpioLedG));
	memset(&GpioLedO, 0, sizeof(GpioLedO));
	memset(&GpioLedR, 0, sizeof(GpioLedR));
	memset(&GpioBtn, 0, sizeof(GpioBtn));

	GpioLedG.pGPIOx 								= GPIOD;
	GpioLedG.GPIO_PinConfig.GPIO_PinNumber 			= GPIO_PIN_NO_12;
	GpioLedG.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	GpioLedG.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_SPEED_HIGH;
	GpioLedG.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OP_TYPE_PP;
	GpioLedG.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_NO_PUPD;

	GpioLedO.pGPIOx 								= GPIOD;
	GpioLedO.GPIO_PinConfig.GPIO_PinNumber 			= GPIO_PIN_NO_13;
	GpioLedO.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	GpioLedO.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_SPEED_HIGH;
	GpioLedO.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OP_TYPE_PP;
	GpioLedO.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_NO_PUPD;

	GpioLedR.pGPIOx 								= GPIOD;
	GpioLedR.GPIO_PinConfig.GPIO_PinNumber 			= GPIO_PIN_NO_14;
	GpioLedR.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	GpioLedR.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_SPEED_HIGH;
	GpioLedR.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OP_TYPE_PP;
	GpioLedR.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_NO_PUPD;

	GpioBtn.pGPIOx									= GPIOA;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber			= GPIO_PIN_NO_0;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IT_RT;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_SPEED_HIGH;
	GpioBtn.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OP_TYPE_PP;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GpioLedG);
	GPIO_Init(&GpioLedO);
	GPIO_Init(&GpioLedR);
	GPIO_Init(&GpioBtn);

	GPIO_IRQInterruptConfig(IRQ_NO_EXTI0, ENABLE);
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI0, 15);


	while(1)
		{
			GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_13);
			delay();
			GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_14);
			delay();
		}
	return 0;
}

void EXTI0_IRQHandler(void){

	delay();
	GPIO_IRQHandling(GPIO_PIN_NO_0);
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
	printf("interrupt cuyyy\n");
}
