/*
 * uartt_caseIT.c
 *
 *  Created on: Oct 1, 2026
 *      Author: EE-11
 */

#include<stdio.h>
#include<string.h>
#include "stm32f407xx.h"



//we have 3 different messages that we transmit to arduino
//you can by all means add more messages
char *msg[3] = {"hihihihihihi123", "Hello How are you ?" , "Today is Monday !"};

//reply from arduino will be stored here
char rx_buf[1024] ;

USART_Handle_t usart6_handle;


//This flag indicates reception completion
uint8_t rxCmplt = RESET;

uint8_t g_data = 0;


void USART6_Init(void)
{
	usart6_handle.pUSARTx = USART6;
	usart6_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	usart6_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	usart6_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
	usart6_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	usart6_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
	usart6_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	USART_Init(&usart6_handle);
}

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

void GPIO_ButtonInit(void)
{
	GPIO_Handle_t GPIOBtn,GpioLed;

	//this is btn gpio configuration
	GPIOBtn.pGPIOx = GPIOA;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GPIOBtn);

	//this is led gpio configuration
	GpioLed.pGPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOD,ENABLE);

	GPIO_Init(&GpioLed);

}

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}
int main(void)
{
	uint32_t cnt = 0;

	GPIO_ButtonInit();
	USART6_GPIOInit();
    USART6_Init();

    USART_IRQInterruptConfig(IRQ_USART6,ENABLE);

    USART_PeripheralControl(USART6,ENABLE);

    printf("Application is running\n");

    //do forever
    while(1)
    {
		//wait till button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA,GPIO_PIN_NO_0) );

		//to avoid button de-bouncing related issues 200ms of delay
		delay();

		// Next message index ; make sure that cnt value doesn't cross 2
		cnt = cnt % 3;

		//First lets enable the reception in interrupt mode
		//this code enables the receive interrupt
		while ( USART_ReceiveDataIT(&usart6_handle,(uint8_t *)rx_buf,strlen(msg[cnt])) != USART_READY );

		//Send the msg indexed by cnt in blocking mode
    	USART_SendData(&usart6_handle,(uint8_t*)msg[cnt],strlen(msg[cnt]));

    	printf("Transmitted : %s\n",msg[cnt]);


    	//Now lets wait until all the bytes are received from the arduino .
    	//When all the bytes are received rxCmplt will be SET in application callback
    	while(rxCmplt != SET);

    	//just make sure that last byte should be null otherwise %s fails while printing
    	rx_buf[strlen(msg[cnt])+ 1] = '\0';

    	//Print what we received from the arduino
    	printf("Received    : %s\n",rx_buf);

    	//invalidate the flag
    	rxCmplt = RESET;

    	//move on to next message indexed in msg[]
    	cnt ++;
    }


	return 0;
}


void USART6_IRQHandler(void)
{
	USART_IRQHandling(&usart6_handle);
}





void USART_ApplicationEventCallback( USART_Handle_t *pUSARTHandle,uint8_t ApEv)
{
   if(ApEv == USART_EVENT_RX_CMPLT)
   {
			rxCmplt = SET;

   }else if (ApEv == USART_EVENT_TX_CMPLT)
   {
	   ;
   }
}
