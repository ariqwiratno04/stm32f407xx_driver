/*
 * i2c_slave_tx_string.c
 *
 *  Created on: Sep 24, 2026
 *      Author: EE-11
 */

#include "stm32f407xx.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define	SLAVE_ADDRESS 	0x68
#define MY_ADDRESS	SLAVE_ADDRESS

/*
 * PB6 --> I2C1_SCL
 * PB7 --> I2C1_SDA
 */

I2C_Handle_t I2C1Handle;

//Tx buffer
uint8_t some_data[32] = "STM32 Slave mode testing";

void delay(void)
{
	for(uint32_t i = 0; i < 500000/2; i++);
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
	I2C1Pins.GPIO_PinConfig.GPIO_PinNumber		= GPIO_PIN_NO_7;
	GPIO_Init(&I2C1Pins);
}

void Green_LED_Inits(void)
{
	GPIO_Handle_t GpioLedG;

	GpioLedG.pGPIOx 								= GPIOD;
	GpioLedG.GPIO_PinConfig.GPIO_PinNumber 			= GPIO_PIN_NO_12;
	GpioLedG.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	GpioLedG.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_SPEED_HIGH;
	GpioLedG.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OP_TYPE_PP;
	GpioLedG.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_NO_PUPD;

	GPIO_Init(&GpioLedG);
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


int main(void)
{

	//I2C pin and Button init
	Button_Inits();
	Green_LED_Inits();
	I2C1_GPIOInits();
	I2C1_Inits();
	printf("Init done\n");

	//I2C IRQ configurations
	I2C_IRQInterruptConfig(IRQ_I2C1_EV,ENABLE);
	I2C_IRQInterruptConfig(IRQ_I2C1_ER,ENABLE);

	I2C_SlaveEnableDisableCallbackEvents(I2C1, ENABLE);

	//Enable the I2C peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	//Enable the ack after PE
	I2C_ManageAcking(I2C1, ENABLE);

	while(1){

	GPIO_ToggleOutputPin(GPIOD, 12);
	delay();

	}

	return 0;
}

void I2C1_EV_IRQHandler (void)
{
	I2C_EV_IRQHandling(&I2C1Handle);
}


void I2C1_ER_IRQHandler (void)
{
	I2C_ER_IRQHandling(&I2C1Handle);
}



void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t AppEv)
{
	static uint8_t commandcode = 0;
	static uint8_t Cnt = 0;

	if(AppEv == I2C_EV_DATA_REQ)
	{
		//send data from slave to master
		if(commandcode == 0x51)
		{
			//send the length information to the master
			I2C_SlaveSendData(pI2CHandle->pI2Cx, strlen((char*)some_data));

		}else if(commandcode == 0x52)
		{
			//send the actual data to master
			I2C_SlaveSendData(pI2CHandle->pI2Cx, some_data[Cnt++]);
		}
	}else if(AppEv == I2C_EV_DATA_RCV)
	{
		//slave has to read
		commandcode = I2C_SlaveReceiveData(pI2CHandle->pI2Cx);

	}else if(AppEv == I2C_ERROR_AF)
	{
		//acknowledge failure, slave should understood that master doesnt need more data
		commandcode = 0xFF;
		Cnt = 0;

	}else if(AppEv == I2C_EV_STOP)
	{
		//only happen during slave reception
		//master has ended the communication
		printf("Comm has stopped\n");
	}
}
