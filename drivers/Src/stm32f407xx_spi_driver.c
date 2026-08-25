/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: Aug 5, 2026
 *      Author: LEGION
 */

#include "stm32f407xx.h"

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);

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

		//Wait until RXNE set (wait until receive buffer is not empty) using while loop
		while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

		//Check the DFF bit in CR1
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			//16 bit data frame
			//Load the data from data register to Rx buffer
			*((uint16_t*)pRxBuffer) = pSPIx->DR;
			Len -= 2;
			pRxBuffer += 2;
		}else
		{
			//8 bit data frame
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

/******************************
 * @fn			: SPI_IRQInterruptConfig
 * @brief		: Configuration init for interrupt IRQ
 *
 * @param[0]	: IRQ Number
 * @param[1]	: Enable or disable
 *
 * @return		: none
 * @Note		: none
 *
 *
 * Interrupt configuration (please refer to the Cortex M4 Reference Manual)
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if(EnorDi == ENABLE){
		if(IRQNumber <= 31){

			//program to ISER0 Register to enable
			*NVIC_ISER0 |= (1 << IRQNumber);

		}else if(IRQNumber > 31 && IRQNumber < 64){

			//program to ISER1 Register to enable
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));

		}else if(IRQNumber >= 64 && IRQNumber < 96){

			//program to ISER2 Register to enable
			*NVIC_ISER2 |= (1 << (IRQNumber % 64));
		}
	}else{
		if(IRQNumber <= 31){

			//program to ICER0 Register to enable
			*NVIC_ICER0 |= (1 << IRQNumber);


		}else if(IRQNumber > 31 && IRQNumber < 64){

			//program to ICER1 Register to enable
			*NVIC_ICER1 |= (1 << IRQNumber);

		}else if(IRQNumber >= 64 && IRQNumber < 96){

			//program to ICER2 Register to enable
			*NVIC_ICER2 |= (1 << IRQNumber);

		}
	}

}

/******************************
 * @fn			: SPI_IRQPriorityConfig
 * @brief		: Configuration init for interrupt IRQ
 *
 * @param[0]	: IRQ Number
 * @param[1]	: IRQ Priority
 *
 * @return		: none
 * @Note		: none
 *
 */
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//Calculate the ipr register from the IRQ number
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t	shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}

void SPI_IRQHandling(SPI_Handle_t *pHandle){

	uint8_t temp1, temp2;

	//check for TXE
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_TXE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

	if(temp1 && temp2){
		//handle TXE
		spi_txe_interrupt_handle(pHandle);
	}

	//check for RXNE
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

	if(temp1 && temp2){
		//handle RXE
		spi_rxne_interrupt_handle(pHandle);
	}

	//check for OVR
		temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_OVR);
		temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

	if(temp1 && temp2){
		//handle OVR error
		spi_ovr_err_interrupt_handle(pHandle);
	}
}

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len){

	uint8_t state = pSPIHandle->TxState;

	if(state != SPI_BUSY_IN_TX)
	{
	//1. Save the TX Buffer address and Len information
	pSPIHandle->pTxBuffer = pTxBuffer;
	pSPIHandle->TxLen = Len;

	//2. Mark the SPI state as busy in transmission
	pSPIHandle->TxState = SPI_BUSY_IN_TX;

	//3. Enable TXEIE control bit to get interrupt whenever TXE flag is st in SR
	pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);

	}

	//4. Data transmission will be handled by the ISR
	return state;
}

uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len){

	uint8_t state = pSPIHandle->RxState;

	if(state != SPI_BUSY_IN_RX)
	{
	//1. Save the TX Buffer address and Len information
	pSPIHandle->pRxBuffer = pRxBuffer;
	pSPIHandle->RxLen = Len;

	//2. Mark the SPI state as busy in transmission
	pSPIHandle->RxState = SPI_BUSY_IN_RX;

	//3. Enable TXEIE control bit to get interrupt whenever TXE flag is st in SR
	pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);

	}

	//4. Data transmission will be handled by the ISR
	return state;
}

/*
 * Helper function
 */
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			//16 bit data frame
			//3. Load the data from Tx buffer into the data register
			pSPIHandle->pSPIx->DR =		*((uint16_t*)pSPIHandle->pTxBuffer);
			pSPIHandle->TxLen -= 2;
			pSPIHandle->pTxBuffer += 2;
		}else
		{
			//8 bit data frame
			pSPIHandle->pSPIx->DR =		*pSPIHandle->pTxBuffer;
			pSPIHandle->TxLen--;
			pSPIHandle->pTxBuffer++;
		}

	if(! pSPIHandle->TxLen)
	{
		//TxLen is zero, so close the SPI transmission and inform the application that Tx is over
		SPI_CloseTransmission(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
	}
}

static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	//Check the DFF bit in CR1
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
	{
		//16 bit data frame
		//Load the data from data register to Rx buffer
		*((uint16_t*)pSPIHandle->pRxBuffer) = pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen -= 2;
		pSPIHandle->pRxBuffer += 2;
	}else
	{
		//8 bit data frame
		*(pSPIHandle->pRxBuffer) = pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--;
		pSPIHandle->pRxBuffer++;
	}

	if(! pSPIHandle->RxLen)
	{
		//Reception is complete
		SPI_CloseReception(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);
	}
}


static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp;
	if(pSPIHandle->TxState != SPI_BUSY_IN_TX)
	{
		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->SR;
	}
	(void)temp;
	SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);
}

void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	//prevents interrupts from setting up of TXE flag
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;
}

void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	//prevents interrupts from setting up of TXE flag
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;
}

void SPI_ClearOVRFlag(SPI_Regdef_t *pSPIx)
{
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void)temp;
}

__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t AppEv)
{

	//This is a weak implementation . the user application may override this function.
}
