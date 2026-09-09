/*
 * i2c_master_tx.c
 *
 *  Created on: Sep 5, 2026
 *      Author: LEGION
 */

/*
 * PB6 --> I2C1_SCL
 * PB7 --> I2C1_SDA
 */

#include "stm32f407xx.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define MY_ADDRESS		0x61
#define	SLAVE_ADDRESS 	0x68

//create data
uint8_t some_data[]	= "We are testing I2C master Tx \n";

I2C_Handle_t I2C1Handle;

void delay(void)
{
	for(uint32_t i = 0; i < 500000; i++);
}

void I2C1_GPIOInits(void){

	GPIO_Handle_t I2C1Pins;

	I2C1Pins.pGPIOx 							= GPIOB;
	I2C1Pins.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALT;
	I2C1Pins.GPIO_PinConfig.GPIO_PinAltFunMode 	= GPIO_AF4;		//I2C1 mode
	I2C1Pins.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OP_TYPE_OD;
	I2C1Pins.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PIN_PU;
	I2C1Pins.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_SPEED_HIGH;

	//Initialize the SCL
	I2C1Pins.GPIO_PinConfig.GPIO_PinNumber		= GPIO_PIN_NO_6;
	GPIO_Init(&I2C1Pins);

	//Initialize the SDA
	I2C1Pins.GPIO_PinConfig.GPIO_PinNumber		= GPIO_PIN_NO_7
			;
	GPIO_Init(&I2C1Pins);

}

void I2C1_Inits(void){


	I2C1Handle.pI2Cx							= I2C1;
	I2C1Handle.I2C_Config.I2C_ACKControl		= I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle		= I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed			= I2C_SCL_SPEED_SM;
	I2C1Handle.I2C_Config.I2C_DeviceAddress		= MY_ADDRESS;

	I2C_Init(&I2C1Handle);

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

	//I2C pin and Button init
	Button_Inits();
	I2C1_GPIOInits();
	I2C1_Inits();

	//Enable the I2C peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	while(1){

	//Wait button press
	while(! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));
	delay();

	//Send the data
	I2C_MasterSendData(&I2C1Handle, some_data, strlen((char*)some_data), SLAVE_ADDRESS);
	}
}
