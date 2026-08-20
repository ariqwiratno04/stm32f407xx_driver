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

	//enable the peripheral clock control
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	//1. Configure the device mode
	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

	//2. Configure the bus mode
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		//BIDI mode cleared (bit 15)
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		//BIDI mode set to 1
		tempreg |= (1 << SPI_CR1_BIDIMODE);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_S_RX)
	{
		//BIDI mode cleared
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
		//Enable the RXONLY bit 10
		tempreg |= (1 << SPI_CR1_RXONLY);
	}

	//3. Configure the SPI baudrate
	tempreg |= (pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR);

	//4. Configure the SPI data frame format
	tempreg |= (pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF);

	//5. Configure the CPOL and CPHA
	tempreg |= (pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL);
	tempreg |= (pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA);

	//6. Configure the Slave Software Management
	tempreg |= (pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM);

	//Set all the configuration bit into the CR1 register
	pSPIHandle->pSPIx->CR1 = tempreg;
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
void SPI_DeInit(SPI_Regdef_t *pSPIx)
{

}

uint8_t SPI_GetFlagStatus(SPI_Regdef_t *pSPIx, uint32_t FlagName)
{
	if(pSPIx->SR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

/******************************
 * @fn			: SPI_SendData
 * @brief		: Send data through SPI with blocking polling loop
 *
 * @param[0]	: SPI Regdef
 * @param[1]	: TX data buffer
 * @param[2]	: Data length
 *
 * @return		: none
 * @Note		: none
 */
void SPI_SendData(SPI_Regdef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
	//Temporary variable for clearing RXNE using dummy read
	volatile uint32_t dummyRead;

	while(Len > 0){

		//1. Wait until TXE set (wait until transmit buffer is empty) using while loop
		while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

		//2. Check the DFF bit in CR1
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			//16 bit data frame
			//3. Load the data from Tx buffer into the data register
			pSPIx->DR =		*((uint16_t*)pTxBuffer);
			Len -= 2;
			pTxBuffer += 2;
		}else
		{
			//8 bit data frame
			pSPIx->DR =		*pTxBuffer;
			Len--;
			pTxBuffer++;
		}

		//4. Wait for the transfer to complete and RXNE to be set
		while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

		// 4. DUMMY READ to clear RXNE and prevent Overrun (OVR)
		dummyRead = pSPIx->DR;
		(void)dummyRead; // Prevents compiler "unused variable" warning

	}
}

/******************************
 * @fn			: SPI_ReceiveData
 * @brief		: Read received data through SPI with blocking polling loop
 *
 * @param[0]	: SPI Regdef
 * @param[1]	: RX data buffer
 * @param[2]	: Data length
 *
 * @return		: none
 * @Note		: none
 */
void SPI_ReceiveData(SPI_Regdef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{

	while(Len > 0){

		//2. Check the DFF bit in CR1
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			//16 bit data frame
			//Send 16-bit dummy data FIRST to generate SPI clocks and shift Slave's data register
			while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);
			pSPIx->DR = 0xFFFF;     // dummy write to generate clock

			//Wait until RXNE set (wait until receive buffer is not empty) using while loop
			while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

			//Load the data from data register to Rx buffer
			*((uint16_t*)pRxBuffer) = pSPIx->DR;
			Len -= 2;
			pRxBuffer += 2;
		}else
		{
			//8 bit data frame
			//Send 8-bit dummy data FIRST to generate SPI clocks and shift Slave's data register
			while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);
			pSPIx->DR = 0xFF;     // dummy write to generate clock

			//Wait until RXNE set (wait until receive buffer is not empty) using while loop
			while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

			*pRxBuffer = pSPIx->DR;
			Len--;
			pRxBuffer++;
		}
	}
}

/******************************
 * @fn			: SPI_PeripheralControl
 * @brief		: Enable the SPI peripheral
 *
 * @param[0]	: SPI Regdef
 * @param[1]	: Enable or disable
 *
 * @Note		: use this function after done configuring the SPI config
 */
void SPI_PeripheralControl(SPI_Regdef_t *pSPIx, uint8_t EnorDi){

	if(EnorDi == ENABLE){

		pSPIx->CR1 |= (1 << SPI_CR1_SPE);		//enable the SPE register

	}else{

		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);		//clear the SPE register

	}
}

/******************************
 * @fn			: SPI_SSIConfig
 * @brief		: Enable the SSI or internal slave select
 *
 * @param[0]	: SPI Regdef
 * @param[1]	: Enable or disable
 *
 * @Note		: use this function for configuring the SPI internal slave select
 * 				(use this SSIConfig when using software SSM)
 */
void SPI_SSIConfig(SPI_Regdef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE){

			pSPIx->CR1 |= (1 << SPI_CR1_SSI);		//enable the SPE register

		}else{

			pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);		//clear the SPE register

		}
}

/******************************
 * @fn			: SPI_SSOEConfig
 * @brief		: Enable the SSM hardware by pulling low as a master
 *
 * @param[0]	: SPI Regdef
 * @param[1]	: Enable or disable
 *
 * @Note		: use this function for configuring the SPI hardware when acting as a master
 * 				(use this SSOEConfig when using hardware SSM)
 */
void SPI_SSOEConfig(SPI_Regdef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE){

			pSPIx->CR2 |= (1 << SPI_CR2_SSOE);		//enable the SSOE register

		}else{

			pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);		//clear the SSOE register

		}
}


