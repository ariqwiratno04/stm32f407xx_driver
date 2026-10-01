/*
 * usart_tx.c
 *
 *  Created on: Sep 29, 2026
 *      Author: EE-11
 */

#include "stm32f407xx.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

/*
 * PC6 -> USART6_TX
 * PC7 -> USART6_RX
 */

USART_Handle_t	USART1Handle;

char msg[1024] = "UART Tx Test... \n\r";

void USART6_GPIOInit(void)
{
	GPIO_Handle_t USART1Pins;

	USART1Pins.pGPIOx								= GPIOC;
	USART1Pins.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALT;
	USART1Pins.GPIO_PinConfig.GPIO_PinAltFunMode	= GPIO_AF8;
	USART1Pins.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OP_TYPE_PP;
	USART1Pins.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_SPEED_HIGH;
	USART1Pins.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PIN_PU;

	//USART6_TX
	USART1Pins.GPIO_PinConfig.GPIO_PinNumber		= GPIO_PIN_NO_6;
	GPIO_Init(&USART1Pins);

	//USART6_RX
	USART1Pins.GPIO_PinConfig.GPIO_PinNumber		= GPIO_PIN_NO_7;
	GPIO_Init(&USART1Pins);
}

void USART6_Init(void)
{
	USART1Handle.pUSARTx							= USART6;
	USART1Handle.USART_Config.USART_Baud			= USART_STD_BAUD_115200;
	USART1Handle.USART_Config.USART_HWFlowControl	= USART_HW_FLOW_CTRL_NONE;
	USART1Handle.USART_Config.USART_Mode			= USART_MODE_ONLY_TX;
	USART1Handle.USART_Config.USART_NoOfStopBits	= USART_STOPBITS_1;
	USART1Handle.USART_Config.USART_WordLength		= USART_WORDLEN_8BITS;
	USART1Handle.USART_Config.USART_ParityControl	= USART_PARITY_DISABLE;

	USART_Init(&USART1Handle);
}


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

	Button_Inits();
	USART6_GPIOInit();
	USART6_Init();

	USART_PeripheralControl(USART6, ENABLE);



	while(1){
		//Wait button press
		while(! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));
		delay();
		printf("Button pressed\n");

		USART_SendData(&USART1Handle,(uint8_t*) msg, strlen(msg));

	}

	return 0;
}
