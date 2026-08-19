/*
 * spi_test_tx_arduino.c
 *
 *  Created on: Aug 14, 2026
 *      Author: LEGION
 */

/*
 * PB14 --> SPI2_MISO
 * PB15 --> SPI2_MOSI
 * PB13	--> SPI2_SCLK
 * PB12 --> SPI2_NSS
 */

#include "stm32f407xx.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define NACK 0xA5
#define ACK 0xF5

//command codes
#define COMMAND_LED_CTRL          	0x50
#define COMMAND_SENSOR_READ       	0x51
#define COMMAND_LED_READ          	0x52
#define COMMAND_PRINT           	0x53
#define COMMAND_ID_READ         	0x54

#define LED_ON     		1
#define LED_OFF    		0

//arduino analog pins
#define ANALOG_PIN0   	0
#define ANALOG_PIN1   	1
#define ANALOG_PIN2   	2
#define ANALOG_PIN3   	3
#define ANALOG_PIN4   	4

void delay(void)
{
	for(uint32_t i = 0; i < 500000; i++);
}


void SPI2_GPIOInits(void){

	GPIO_Handle_t SPIPins;

	SPIPins.pGPIOx 								= GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALT;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode 	= GPIO_AF5;		//SPI mode
	SPIPins.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_SPEED_HIGH;

	//Initialize the MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber		= GPIO_PIN_NO_14;
	GPIO_Init(&SPIPins);

	//Initialize the MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber		= GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);

	//Initialize the SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber		= GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	//Initialize the NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber		= GPIO_PIN_NO_12;
	GPIO_Init(&SPIPins);

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

void SPI2_Inits(void){

	SPI_Handle_t SPI2Handle;

	SPI2Handle.pSPIx 							= SPI2;
	SPI2Handle.SPIConfig.SPI_BusConfig			= SPI_BUS_CONFIG_FD;
	SPI2Handle.SPIConfig.SPI_DFF				= SPI_DFF_8BIT;
	SPI2Handle.SPIConfig.SPI_DeviceMode			= SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_CPHA				= SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_CPOL				= SPI_CPOL_LOW;
	SPI2Handle.SPIConfig.SPI_SclkSpeed			= SPI_SCLK_SPEED_DIV8;
	SPI2Handle.SPIConfig.SPI_SSM				= SPI_SSM_DI;		//will be using SS hardware

	SPI_Init(&SPI2Handle);
}

uint8_t SPI_VerifyAck(uint8_t AckByte){

	if(AckByte == ACK){
		return 1;
	}else{
		return 0;
	}
}

int main(void){

	//char user_data[] = "Hello World";
	uint8_t LED_PIN = 0;

	Button_Inits();
	SPI2_GPIOInits();
	SPI2_Inits();
	SPI_SSOEConfig(SPI2, ENABLE);

	while(1)
	{

		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));
		delay();		//button debouncer

		SPI_PeripheralControl(SPI2, ENABLE);			//enable the SPi after SPI inits

		//1. Send CMD_LED_CTRL consists <Pin_Number> , <EnorDI>
		uint8_t commandcode = COMMAND_LED_CTRL;
		SPI_SendData(SPI2, &commandcode, 1);

		//2. Read the ACK byte
		uint8_t ackbyte;
		SPI_ReceiveData(SPI2, &ackbyte, 1);

		//3. Verify ACK and send the rest of the command
		uint8_t args[2];
		if(SPI_VerifyAck(ackbyte)){

			args[0] = LED_PIN;
			args[1] = ENABLE;
			SPI_SendData(SPI2, args, 2);
		}

		while(SPI_GetFlagStatus(SPI2, SPI_BSY_FLAG));	//wait until data is sent
		SPI_PeripheralControl(SPI2, DISABLE);			//disable after all the data are send
	}


	return 0;
}
