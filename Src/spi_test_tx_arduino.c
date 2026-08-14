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

int main(void){

	char user_data[] = "Hello World";

	SPI2_GPIOInits();
	SPI2_Inits();

	SPI_PeripheralControl(SPI2, ENABLE);

	while(1){
	SPI_SendData(SPI2, (uint8_t*)user_data, strlen(user_data));
	}

	return 0;
}
