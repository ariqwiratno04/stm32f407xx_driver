/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: Aug 5, 2026
 *      Author: LEGION
 */

#include "stm32f407xx_gpio_driver.h"

/*
 * Peripheral clock setup
 */

/******************************
 * @fn			: SPI_PeriClockControl
 * @brief		: Enable peripheral clock of SPI
 *
 * @param[0]	: SPI Port
 * @param[1]	: Enable or disable macros
 *
 * @return		: none
 * @Note		: none
 */
void SPI_PeriClockControl(SPI_Regdef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pSPIx == SPI1){
			SPI1_PCLK_EN();
		}
		else if(pSPIx == SPI2){
			SPI2_PCLK_EN();
		}
		else if(pSPIx == SPI3){
			SPI3_PCLK_EN();
		}
		else if(pSPIx == SPI4){
			SPI4_PCLK_EN();
		}

	}
	else if(EnorDi == DISABLE)
	{
		if(pSPIx == SPI1){
			SPI1_PCLK_DI();
		}
		else if(pSPIx == SPI2){
			SPI2_PCLK_DI();
		}
		else if(pSPIx == SPI3){
			SPI3_PCLK_DI();
		}
		else if(pSPIx == SPI4){
			SPI4_PCLK_DI();
		}
	}
}


