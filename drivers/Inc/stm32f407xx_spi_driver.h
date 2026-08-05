/*
 * stm32f407xx_spi_driver.h
 *
 *  Created on: Aug 5, 2026
 *      Author: LEGION
 */

#ifndef INC_STM32F407XX_SPI_DRIVER_H_
#define INC_STM32F407XX_SPI_DRIVER_H_

#include "stm32f407xx.h"

/*
 * Config structure for a SPI hardware
 */
typedef struct{

	uint8_t SPI_DeviceMode;
	uint8_t SPI_BUSConfig;
	uint8_t SPISclkSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;

}SPI_PinConfig_t;

typedef struct{

	SPI_Regdef_t	*pSPIx;
	SPI_Config_t	SPIConfig;

}SPI_Handle_t;

/********************************************************
 * APIs Supported by this driver prototypes
 *******************************************************/

/*
 * Peripheral clock setup
 */
void SPI_PeriClockControl(SPI_Regdef_t *pSPIx, uint8_t EnorDi);

/*
 * Initialize and De-initialize
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_Regdef_t *pSPIx);

/*
 * Data Send and Receive
 */
void SPI_SendData(SPI_Regdef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_Regdef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);

/*
 * IRQ configuration and ISR handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pHandle);


/*
 * Other
 */


#endif /* INC_STM32F407XX_SPI_DRIVER_H_ */
