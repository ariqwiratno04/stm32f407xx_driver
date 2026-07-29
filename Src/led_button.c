#include <stdint.h>
#include "stm32f407xx.h"

void delay(void)
{
	for(uint32_t i = 0; i < 500000; i++);
}


int main(void){

	GPIO_Handle_t GpioLedG, GpioButton;

	GpioLedG.pGPIOx 								= GPIOD;
	GpioLedG.GPIO_PinConfig.GPIO_PinNumber 			= GPIO_PIN_NO_12;
	GpioLedG.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	GpioLedG.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_SPEED_HIGH;
	GpioLedG.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OP_TYPE_PP;
	GpioLedG.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_NO_PUPD;

	GpioButton.pGPIOx 								= GPIOA;
	GpioButton.GPIO_PinConfig.GPIO_PinNumber 		= GPIO_PIN_NO_0;
	GpioButton.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_IN;
	GpioButton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_SPEED_HIGH;
	GpioButton.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GpioLedG);
	GPIO_Init(&GpioButton);

	while(1){
		if(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) == SET){
			delay();
			GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
		}
	}
	return 0;
}
