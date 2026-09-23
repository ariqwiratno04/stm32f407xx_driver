/*
 * i2c_master_rx_testingIT.c
 *
 *  Created on: Sep 22, 2026
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

#define MY_ADDRESS		0x61
#define	SLAVE_ADDRESS 	0x68

extern void initialise_monitor_handles(void);

//create Rx buffer
uint8_t some_data[32];

//Flag variable
uint8_t rxComplt = RESET;

I2C_Handle_t I2C1Handle;

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

	uint8_t commandcode;
	uint8_t len;

	initialise_monitor_handles();

	//I2C pin and Button init
	Button_Inits();
	I2C1_GPIOInits();
	I2C1_Inits();
	//printf("Init done \n");

	//I2C IRQ configurations
	I2C_IRQInterruptConfig(IRQ_I2C1_EV,ENABLE);
	I2C_IRQInterruptConfig(IRQ_I2C1_ER,ENABLE);

	//Enable the I2C peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	//Enable the ack after PE
	I2C_ManageAcking(I2C1, ENABLE);

	while(1){

		//Wait button press
		while(! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));
		delay();
		//printf("Button pressed \n");

		//Send slave command code to send data length
		commandcode = 0x51;

		while(I2C_MasterSendDataIT(&I2C1Handle, &commandcode, 1, SLAVE_ADDRESS, I2C_ENABLE_SR) != I2C_READY);

		//receive data length from slave
		while(I2C_MasterReceiveDataIT(&I2C1Handle, &len, 1, SLAVE_ADDRESS, I2C_ENABLE_SR) != I2C_READY);

		//Send slave command code to send data
		commandcode = 0x52;
		while(I2C_MasterSendDataIT(&I2C1Handle, &commandcode, 1, SLAVE_ADDRESS, I2C_ENABLE_SR) != I2C_READY);

		//receive data from slave
		while(I2C_MasterReceiveDataIT(&I2C1Handle, some_data, len, SLAVE_ADDRESS, I2C_DISABLE_SR) != I2C_READY);

		rxComplt = RESET;

		//wait till rx completes
		while(rxComplt != SET)
		{

		}

		some_data[len] = '\0';

		printf("Data : %s \n", some_data);

		rxComplt = RESET;
	}
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
     if(AppEv == I2C_EV_TX_CMPLT)
     {
    	 printf("Tx is completed\n");
     }else if (AppEv == I2C_EV_RX_CMPLT)
     {
    	 printf("Rx is completed\n");
    	 rxComplt = SET;
     }else if (AppEv == I2C_ERROR_AF)
     {
    	 printf("Error : Ack failure\n");
    	 //in master ack failure happens when slave fails to send ack for the byte
    	 //sent from the master.
    	 I2C_CloseSendData(pI2CHandle);

    	 //generate the stop condition to release the bus
    	 I2C_GenerateStopCondition(I2C1);

    	 //Hang in infinite loop
    	 while(1);
     }
}
