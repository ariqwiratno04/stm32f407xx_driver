/*
 * stm32f407xx_gpio_driver.c
 *
 *  Created on: Jul 26, 2026
 *      Author: LEGION
 */

#include "stm32f407xx_gpio_driver.h"

/*
 * Peripheral clock setup
 */

/******************************
 * @fn			: GPIO_PeriClockControl
 * @brief		: Enable peripheral clock of GPIO
 *
 * @param[0]	: GPIO Port
 * @param[1]	: Enable or disable macros
 *
 * @return		: none
 * @Note		: none
 */
void GPIO_PeriClockControl(GPIO_Regdef_t *pGPIOx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pGPIOx == GPIOA){
			GPIOA_PCLK_EN();
		}
		else if(pGPIOx == GPIOB){
			GPIOB_PCLK_EN();
		}
		else if(pGPIOx == GPIOC){
			GPIOC_PCLK_EN();
		}
		else if(pGPIOx == GPIOD){
			GPIOD_PCLK_EN();
		}
		else if(pGPIOx == GPIOE){
			GPIOE_PCLK_EN();
		}
		else if(pGPIOx == GPIOF){
			GPIOF_PCLK_EN();
				}
		else if(pGPIOx == GPIOG){
			GPIOG_PCLK_EN();
		}
		else if(pGPIOx == GPIOH){
			GPIOH_PCLK_EN();
		}
		else if(pGPIOx == GPIOI){
			GPIOI_PCLK_EN();
		}
	}
	else if(EnorDi == DISABLE)
		{
			if(pGPIOx == GPIOA){
				GPIOA_PCLK_DI();
			}
			else if(pGPIOx == GPIOB){
				GPIOB_PCLK_DI();
			}
			else if(pGPIOx == GPIOC){
				GPIOC_PCLK_DI();
			}
			else if(pGPIOx == GPIOD){
				GPIOD_PCLK_DI();
			}
			else if(pGPIOx == GPIOE){
				GPIOE_PCLK_DI();
			}
			else if(pGPIOx == GPIOF){
				GPIOF_PCLK_DI();
					}
			else if(pGPIOx == GPIOG){
				GPIOG_PCLK_DI();
			}
			else if(pGPIOx == GPIOH){
				GPIOH_PCLK_DI();
			}
			else if(pGPIOx == GPIOI){
				GPIOI_PCLK_EN();
			}
		}
}

/*
 * Initialize and De-initialize
 */
/******************************
 * @fn			: GPIO_Init
 * @brief		: Enable peripheral clock of GPIO
 *
 * @param[0]	: GPIO Base Address
 * @param[1]	: Enable or disable macros
 *
 * @return		: none
 * @Note		: none
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp = 0; //temporary register
	//1. Configure the mode
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG) //Non-interrupt mode
	{
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); //set register and shifting by 2 bit times pin number
		pGPIOHandle->pGPIOx->MODER &= ~(0x03 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);	//clearing register
		pGPIOHandle->pGPIOx->MODER |= temp;		//set register
	}else
	{
		//Code later for interrupt mode
	}
	temp = 0;

	//2. Configure the speed
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x03 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);		//clearing register
	pGPIOHandle->pGPIOx->OSPEEDR |= temp; 		//set register

	temp = 0;

	//3. Configure the pullup or pulldown setting
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x03 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);		//clearing register
	pGPIOHandle->pGPIOx->PUPDR |= temp;			//set register

	temp = 0;

	//4. Configure the output type
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x03 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);		//clearing register
	pGPIOHandle->pGPIOx->OTYPER |= temp;		//set register

	temp = 0;

	//5. Configure the alternate function
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALT)
	{
	uint8_t temp1, temp2;

	temp1 = (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) / 8;
	temp2 = (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) % 8;

		if(temp1 == 0){
			pGPIOHandle->pGPIOx->AFRL &= ~(0xF << (4 * temp2));		//clear register first
			pGPIOHandle->pGPIOx->AFRL |= pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2);
		}
		else{
			pGPIOHandle->pGPIOx->AFRH &= ~(0xF << (4 * temp2));		//clear register first
			pGPIOHandle->pGPIOx->AFRH |= pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2);
		}
	}


	temp = 0;
}

/******************************
 * @fn			: GPIO_DeInit
 * @brief		: Reset GPIO state
 *
 * @param[0]	: GPIO Port
 *
 * @return		: none
 * @Note		: none
 */
void GPIO_DeInit(GPIO_Regdef_t *pGPIOx)
{
	if(pGPIOx == GPIOA){
		GPIOA_REG_RESET();
	}
	else if(pGPIOx == GPIOB){
		GPIOB_REG_RESET();
	}
	else if(pGPIOx == GPIOC){
		GPIOC_REG_RESET();
	}
	else if(pGPIOx == GPIOD){
		GPIOD_REG_RESET();
	}
	else if(pGPIOx == GPIOE){
		GPIOE_REG_RESET();
	}
	else if(pGPIOx == GPIOF){
		GPIOF_REG_RESET();
	}
	else if(pGPIOx == GPIOG){
		GPIOG_REG_RESET();
	}
	else if(pGPIOx == GPIOH){
		GPIOH_REG_RESET();
	}
	else if(pGPIOx == GPIOI){
		GPIOI_REG_RESET();
	}
}

/*
 * Data read and write
 */
/******************************
 * @fn			: GPIO_ReadFromInputPin
 * @brief		: Read from GPIO pin
 *
 * @param[0]	: GPIO Port
 * @param[1]	: GPIO Pin Number
 *
 * @return		: Read value from GPIO Pin
 * @Note		: none
 */
uint8_t GPIO_ReadFromInputPin(GPIO_Regdef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value;
	value = (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001 );		//shift the input bit to LSB and then bitmask the other bit
	return value;
}

/******************************
 * @fn			: GPIO_ReadFromInputPort
 * @brief		: Read from GPIO port
 *
 * @param[0]	: GPIO Port
 *
 * @return		: Read value from GPIO Port
 * @Note		: none
 */
uint16_t GPIO_ReadFromInputPort(GPIO_Regdef_t *pGPIOx)
{
	uint16_t value;
	value = (uint16_t)(pGPIOx->IDR);		//Return the entire bitmask port
	return value;
}

/******************************
 * @fn			: GPIO_WriteToOutputPin
 * @brief		: Write to output pin
 *
 * @param[0]	: GPIO Port
 * @param[1]	: GPIO Pin Number
 * @param[2]	: GPIO Write value
 *
 * @return		: none
 * @Note		: none
 */
void GPIO_WriteToOutputPin(GPIO_Regdef_t *pGPIOx, uint8_t PinNumber, uint8_t PinValue)
{
	if(PinValue == GPIO_PIN_SET)
	{
		pGPIOx->ODR |= (1 << PinNumber); //set to 1 if GPIO_PIN_SET
	}else{
		pGPIOx->ODR &= ~(1 << PinNumber); //clear if GPIO_PIN_RESET
	}
}

/******************************
 * @fn			: GPIO_WriteToOutputPort
 * @brief		: Write to output pin
 *
 * @param[0]	: GPIO Port
 * @param[1]	: GPIO Write Port value
 *
 * @return		: none
 * @Note		: none
 */
void GPIO_WriteToOutputPort(GPIO_Regdef_t *pGPIOx, uint16_t PortValue)
{
	pGPIOx->ODR = (PortValue);
}

/******************************
 * @fn			: GPIO_ToggleOutputPin
 * @brief		: Toggle write to output pin
 *
 * @param[0]	: GPIO Port
 * @param[1]	: GPIO Pin Number
 *
 * @return		: none
 * @Note		: none
 */
void GPIO_ToggleOutputPin(GPIO_Regdef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR ^= (1 << PinNumber);		//Toggle the output pin by using XOR operation
}

/*
 * Interrupt
 */
void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDi);
void GPIO_IRQHandling(uint8_t PinNumber);
