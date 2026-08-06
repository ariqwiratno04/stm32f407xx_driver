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

	uint8_t SPI_DeviceMode;				//Possible values are @SPI_DEVICE_MODE
	uint8_t SPI_BuSConfig;				//Possible values are @SPI_BUS_CONFIG
	uint8_t SPISclkSpeed;				//Possible values are @SPI_SCLK_SPEED
	uint8_t SPI_DFF;					//Possible values are @SPI_DFF
	uint8_t SPI_CPOL;					//Possible values are @SPI_CPOL
	uint8_t SPI_CPHA;					//Possible values are @SPI_CPHA
	uint8_t SPI_SSM;					//Possible values are @SPI_SSM

}SPI_PinConfig_t;

typedef struct{

	SPI_Regdef_t	*pSPIx;
	SPI_PinConfig_t	SPIConfig;

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

/*
 * @SPI_DEVICE_MODE
 */
#define SPI_DEVICE_MODE_SLAVE			0			//Slave mode
#define SPI_DEVICE_MODE_MASTER			1			//Master mode

/*
 *  @SPI_BUS_CONFIG
 */
#define	SPI_BUS_CONFIG FD				0			//Full duplex
#define SPI_BUS_CONFIG_HD				1			//Half duplex
#define SPI_BUS_CONFIG_S_RX				2			//Simplex RX only

/*
 * @SPI_SCLK_SPEED
 */
#define SPI_SCLK_SPEED_DIV2				0			//PCLK divided by 2
#define SPI_SCLK_SPEED_DIV4				1			//PCLK divided by 4
#define SPI_SCLK_SPEED_DIV8				2			//PCLK divided by 8
#define SPI_SCLK_SPEED_DIV16			3			//PCLK divided by 16
#define SPI_SCLK_SPEED_DIV32			4			//PCLK divided by 32
#define SPI_SCLK_SPEED_DIV64			5			//PCLK divided by 64
#define SPI_SCLK_SPEED_DIV128			6			//PCLK divided by 128
#define SPI_SCLK_SPEED_DIV256			7			//PCLK divided by 256

/*
 * @SPI_DFF
 */
#define SPI_DFF_8BIT					0			//8 bit data frame format
#define SPI_DFF_16BIT					1			//16 bit data frame format

/*
 * @SPI_CPOL
 */
#define SPI_CPOL_LOW					0			//clock low when idle
#define SPI_CPOL_HIGH					1			//clock high when idle

/*
 * @SPI_CPHA
 */
#define SPI_CPHA_LOW					0			//The first clock transition is the first data capture edge
#define SPI_CPHA_HIGH					1			//The second clock transition is the first data capture edge

/*
 * @SPI_SSM
 */
#define SPI_SSM_DI					0				//Software slave management disable
#define SPI_SSM_EN					1				//Software slave management enable


#endif /* INC_STM32F407XX_SPI_DRIVER_H_ */
