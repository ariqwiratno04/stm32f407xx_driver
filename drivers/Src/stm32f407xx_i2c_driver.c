/*
 * stm32f407xx_i2c_driver.c
 *
 *  Created on: Aug 26, 2026
 *      Author: EE-11
 */

#include "stm32f407xx.h"

uint16_t AHB_PreScaler[8] = {2, 4, 8, 16, 32, 64, 128, 256, 512};
uint16_t APB1_PreScaler[4] = {2, 4, 8, 16};
/*
 * Peripheral clock setup
 */
void I2C_PeriClockControl(I2C_Regdef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
		{
			if(pI2Cx == I2C1){
				I2C1_PCLK_EN();
			}
			else if(pI2Cx == I2C2){
				I2C2_PCLK_EN();
			}
			else if(pI2Cx == I2C3){
				I2C3_PCLK_EN();
			}
		}
		else if(EnorDi == DISABLE)
		{
			if(pI2Cx == I2C1){
				I2C1_PCLK_DI();
			}
			else if(pI2Cx == I2C2){
				I2C2_PCLK_DI();
			}
			else if(pI2Cx == I2C3){
				I2C3_PCLK_DI();
			}
		}
}

/*
 * Initialize and De-initialize
 */
uint32_t RCC_GetPCLK1Value(void)
{
	uint32_t pclk1;
	uint32_t SystemClk;
	uint8_t clksrc, temp, ahbp, apb1p;

	clksrc = ((RCC->CFGR >> 2) & 0x3);			//shift the CFGR bit to right shift and then bitmask the rest by using AND

	if(clksrc == 0){
		SystemClk = 16000000;
	}else if(clksrc == 1){
		SystemClk = 8000000;
	}else if(clksrc == 2){
		SystemClk = RCC_GetPLLOutputClock();
	}

	//ahbp
	temp = ((RCC->CFGR >> 4) & 0xF);
	if(temp < 8){
		ahbp = 1;
	}else{
		ahbp = AHB_PreScaler[temp-8];
	}

	//apb1p
	temp = ((RCC->CFGR >> 10) & 0x7);
	if(temp < 4){
		apb1p = 1;
	}else{
		apb1p = APB1_PreScaler[temp-8];
	}

	pclk1 = (SystemClk/ahbp) / apb1p;

	return pclk1;
}

uint32_t RCC_GetPLLOutputClock(void){
	uint32_t PLL;
	return PLL;
}

void I2C_Init(I2C_Handle_t *pI2CHandle){

	//Enable the peripheral clock control
	I2C_PeriClockControl(pI2CHandle->pI2Cx, ENABLE);

	//1. Config the I2C master mode selection
	if(pI2CHandle->I2C_Config->I2C_SCLSpeed > 0){

		pI2CHandle->pI2Cx->CCR |= (1 << I2C_CCR_FS);
	}

}

void I2C_DeInit(I2C_Regdef_t *pI2Cx);

/*
 * Data Send and Receive
 */


/*
 * Peripheral control I2C
 */
void I2C_PeripheralControl(I2C_Regdef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE){

		pI2Cx->CR1 |= (1 << I2C_CR1_PE);		//enable the PE register

	}else{

		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);		//clear the PE register

	}
}

/*
 * IRQ configuration and ISR handling
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);

/*
 * Other
 */
uint8_t I2C_GetFlagStatus(I2C_Regdef_t *pI2Cx, uint32_t FlagName);

