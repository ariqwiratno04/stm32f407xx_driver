/*
 * stm32f407xx_gpio_driver.h
 *
 *  Created on: Jul 26, 2026
 *      Author: Ariq
 */

#ifndef INC_STM32F407XX_GPIO_DRIVER_H_
#define INC_STM32F407XX_GPIO_DRIVER_H_

#include "stm32f407xx.h"


/*
 * Config structure for a GPIO pin
 */
typedef struct{

	uint8_t GPIO_PinNumber;					//Posibble values are @GPIO_PIN_NUMBERS
	uint8_t GPIO_PinMode;					//Possible values are @GPIO_PIN_MODE
	uint8_t GPIO_PinSpeed;					//Possible values are @GPIO_PIN_SPEED
	uint8_t GPIO_PinPuPdControl;			//Possible values are @GPIO_PIN_PUPD
	uint8_t GPIO_PinOPType;					//Possible values are @GPIO_PIN_OP_TYPE
	uint8_t GPIO_PinAltFunMode;				//Possible values are @GPIO_PIN_AF_MODE

}GPIO_PinConfig_t;

/*
 * Handle structure for a GPIO pin
 */
typedef struct{

	GPIO_Regdef_t *pGPIOx;					//holds base address of GPIO port
	GPIO_PinConfig_t GPIO_PinConfig;		//holds GPIO Pin Config settings

}GPIO_Handle_t;

/********************************************************
 * APIs Supported by this driver prototypes
 *******************************************************/

/*
 * Peripheral clock setup
 */
void GPIO_PeriClockControl(GPIO_Regdef_t *pGPIOx, uint8_t EnorDi);

/*
 * Initialize and De-initialize
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_Regdef_t *pGPIOx);

/*
 * Data read and write
 */
uint8_t GPIO_ReadFromInputPin(GPIO_Regdef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_Regdef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_Regdef_t *pGPIOx, uint8_t PinNumber, uint8_t PinValue);
void GPIO_WriteToOutputPort(GPIO_Regdef_t *pGPIOx, uint16_t PortValue);
void GPIO_ToggleOutputPin(GPIO_Regdef_t *pGPIOx, uint8_t PinNumber);

/*
 * Interrupt
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);

/*
 * Generic macros for specific driver
 */
//@GPIO_PIN_NUMBERS
#define GPIO_PIN_NO_0			0
#define GPIO_PIN_NO_1			1
#define GPIO_PIN_NO_2			2
#define GPIO_PIN_NO_3			3
#define GPIO_PIN_NO_4			4
#define GPIO_PIN_NO_5			5
#define GPIO_PIN_NO_6			6
#define GPIO_PIN_NO_7			7
#define GPIO_PIN_NO_8			8
#define GPIO_PIN_NO_9			9
#define GPIO_PIN_NO_10			10
#define GPIO_PIN_NO_11			11
#define GPIO_PIN_NO_12			12
#define GPIO_PIN_NO_13			13
#define GPIO_PIN_NO_14			14
#define GPIO_PIN_NO_15			15


//@GPIO_PIN_MODES
#define GPIO_MODE_IN			0
#define GPIO_MODE_OUT			1
#define GPIO_MODE_ALT			2
#define GPIO_MODE_ANALOG		3
#define GPIO_MODE_IT_FT			4
#define GPIO_MODE_IT_RT			5
#define GPIO_MODE_IT_FT_RT		6

//@GPIO_PIN_OP_TYPE
#define GPIO_OP_TYPE_PP			0
#define GPIO_OP_TYPE_OD			1

//@GPIO_PIN_SPEED
#define GPIO_SPEED_LOW			0
#define GPIO_SPEED_MEDIUM		1
#define GPIO_SPEED_HIGH			2
#define GPIO_SPEED_VERY_HIGH	3

//@GPIO_PIN_PUPD (pullup or pulldown)
#define GPIO_NO_PUPD			0
#define GPIO_PIN_PU				1
#define GPIO_PIN_PD				2

//@GPIO_PIN_AF_MODE
#define GPIO_AF0				0
#define GPIO_AF1				1
#define GPIO_AF2				2
#define GPIO_AF3				3
#define GPIO_AF4				4
#define GPIO_AF5				5
#define GPIO_AF6				6
#define GPIO_AF7				7
#define GPIO_AF8				8
#define GPIO_AF9				9
#define GPIO_AF10				10
#define GPIO_AF11				11
#define GPIO_AF12				12
#define GPIO_AF13				13
#define GPIO_AF14				14
#define GPIO_AF15				15

#endif /* INC_STM32F407XX_GPIO_DRIVER_H_ */
