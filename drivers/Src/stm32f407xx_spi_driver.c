/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: Aug 5, 2026
 *      Author: LEGION
 */

#include "stm32f407xx.h"

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

/*
 * Initialize and De-initialize
 */
/******************************
 * @fn			: SPI_Init
 * @brief		: Initialize SPI config
 *
 * @param[0]	: SPI Handle
 *
 * @return		: none
 * @Note		: none
 */
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	uint32_t tempreg = 0;

	//1. Configure the device mode
	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << 2;

	//2. Configure the bus mode

}

/******************************
 * @fn			: SPI_DeInit
 * @brief		: De-Initialize SPI config
 *
 * @param[0]	: SPI Handle
 *
 * @return		: none
 * @Note		: none
 */
void SPI_DeInit(SPI_Regdef_t *pSPIx);


