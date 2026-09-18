/*
 * stm32f407xx_i2c_driver.c
 *
 *  Created on: Aug 26, 2026
 *      Author: EE-11
 */

#include "stm32f407xx.h"

uint16_t AHB_PreScaler[8] = {2, 4, 8, 16, 64, 128, 256, 512};
uint16_t APB1_PreScaler[4] = {2, 4, 8, 16};

//static specific I2C function
static void I2C_GenerateStartCondition(I2C_Regdef_t *pI2Cx);
static void I2C_GenerateStopCondition(I2C_Regdef_t *pI2Cx);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_ExecuteAddressPhaseWrite(I2C_Regdef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_ExecuteAddressPhaseRead(I2C_Regdef_t *pI2Cx, uint8_t SlaveAddr);

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

	uint32_t tempreg = 0;

	//Enable the peripheral clock control
	I2C_PeriClockControl(pI2CHandle->pI2Cx, ENABLE);

	//Config the ACK control bit
	tempreg |= pI2CHandle->I2C_Config.I2C_ACKControl << 10;
	pI2CHandle->pI2Cx->CR1 = tempreg;

	//Config FREQ register CR2
	tempreg |= RCC_GetPCLK1Value() / 1000000U;
	pI2CHandle->pI2Cx->CR2 = (tempreg & 0x3F);

	//Config the device own Address
	tempreg = 0;
	tempreg |= pI2CHandle->I2C_Config.I2C_DeviceAddress << I2C_OAR1_ADD71;	//start writing address by the 1st bit
	tempreg |= (1 << 14);					//need to always 1 by software refer to RM
	pI2CHandle->pI2Cx->OAR1 = tempreg;		//write to register

	//CCR Calculations
	uint16_t ccr_value = 0;
	tempreg = 0;

	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{
		//standard mode
		ccr_value = RCC_GetPCLK1Value() / (2 * pI2CHandle->I2C_Config.I2C_SCLSpeed);		//the formula is RCC/2*SCLSpeed
		tempreg |= (ccr_value & 0xFFF);

	}else{
		//fast mode
		tempreg |= 1 << I2C_CCR_FS;		//set F/S bit to FM mode
		tempreg |= pI2CHandle->I2C_Config.I2C_FMDutyCycle << I2C_CCR_DUTY;		//write the duty cycle
		if(pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2)			//if duty cycle == 0
		{
			ccr_value = (RCC_GetPCLK1Value() / ( 3 * pI2CHandle->I2C_Config.I2C_SCLSpeed ) );
		}else{
			ccr_value = (RCC_GetPCLK1Value() / ( 25 * pI2CHandle->I2C_Config.I2C_SCLSpeed ) );
		}
		tempreg |= (ccr_value & 0xFFF);
	}
	pI2CHandle->pI2Cx->CCR = tempreg;

	//TRISE config calculation
	uint8_t trise;
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
		{
			//standard mode
		trise = (RCC_GetPCLK1Value() / 1000000U) + 1;

		}else{
			//fast mode
		trise = ((RCC_GetPCLK1Value() * 300) / 1000000000U) + 1;
		}
	pI2CHandle->pI2Cx->TRISE |= (trise & 0x3F);
}

void I2C_DeInit(I2C_Regdef_t *pI2Cx);

/*
 * Data Send and Receive
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	//1.Generate start condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. Confirm start generation is completed by checking the SB flag in SR1
	while( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));

	//3. Send the address of the slave with R/W bit, set to W(0) so the total is 8 bits
	I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, SlaveAddr);

	//4. Confirm that address phase is completed by checking the ADDR flag
	while( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

	//5. Clear ADDR Flag according to its software sequence
	I2C_ClearADDRFlag(pI2CHandle);

	//6. Send the data until Len becomes 0
	while (Len > 0)
	{
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));		//wait until TXE set (DR empty)
		pI2CHandle->pI2Cx->DR = *pTxbuffer;
		pTxbuffer++;
		Len--;
	}

	//7. When Len becomes zero wit for TXE=1 and BTF=1 before generating the stop condition
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));

	//8. Generate stop condition
	if(Sr == I2C_DISABLE_SR){
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
	}
}

void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxbuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	//1. Generate start condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. Confirm start generation is completed by checking the SB flag in SR1.
	//until SB is cleared SCL will be stretched
	while( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));

	//3. Send the address of the slave with R/W bit, set to R(1) so the total is 8 bits
	I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, SlaveAddr);

	//4. Confirm that address phase is completed by checking the ADDR flag
	while( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

	//procedure to read only 1 byte from Slave
	if(Len == 1)
	{
		//disable acking
		I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);

		//clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//wait until RXNE becomes 1
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

		//generate stop condition
		if(Sr == I2C_DISABLE_SR){
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}

		//read data into buffer
		*pRxbuffer = pI2CHandle->pI2Cx->DR;
	}

	//procedure to read data from slave when Len > 1
	if(Len > 1)
	{
		//clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//read the data until Len becomes zero
		for(int i = Len; i > 0; i--)
		{
			//wait until RXNE becomes 1
			while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

			if(i == 2){

				//clear the ack bit
				I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);

				//generate stop condition
				I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
			}

			//read data from data register (DR) to buffer
			*pRxbuffer = pI2CHandle->pI2Cx->DR;

			//increment the buffer address
			pRxbuffer++;

		}

	}

	//re-enable acking
	if(pI2CHandle->I2C_Config.I2C_ACKControl == ENABLE){
		I2C_ManageAcking(pI2CHandle->pI2Cx, ENABLE);
	}
}

/*
 * Data send and receive interrupt base
 */
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	uint8_t busystate = pI2CHandle->TxRxState;

		if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
		{
			pI2CHandle->pTxBuffer = pTxbuffer;
			pI2CHandle->TxLen = Len;
			pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
			pI2CHandle->DevAddr = SlaveAddr;
			pI2CHandle->Sr = Sr;

			//Implement code to Generate START Condition
			I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

			//Implement the code to enable ITBUFEN Control Bit
			pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

			//Implement the code to enable ITEVFEN Control Bit
			pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);

			//Implement the code to enable ITERREN Control Bit
			pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);

		}

		return busystate;
}

uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxbuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{

	uint8_t busystate = pI2CHandle->TxRxState;

		if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
		{
			pI2CHandle->pRxBuffer = pRxbuffer;
			pI2CHandle->RxLen = Len;
			pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
			pI2CHandle->RxSize = Len; //Rxsize is used in the ISR code to manage the data reception
			pI2CHandle->DevAddr = SlaveAddr;
			pI2CHandle->Sr = Sr;

			//Implement code to Generate START Condition
			I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

			//Implement the code to enable ITBUFEN Control Bit
			pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

			//Implement the code to enable ITEVFEN Control Bit
			pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);

			//Implement the code to enable ITERREN Control Bit
			pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);
		}

	return busystate;
}


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

void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	uint32_t temp1, temp2, temp3;

	temp1	= pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITEVTEN);
	temp2	= pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN);

	temp3 	= pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_SB);
	//1.Handle for interrupt generated by SB event
	//Only applicable in Master mode
	if(temp1 && temp3)					// for enabling SB flag, ITEVTEN should enabled first
	{
		//SB Flag is set
		//execute address phase
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}

	}

	temp3 	= pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_ADDR);
	//2. Handle for interrupt generated by ADDR event
	//	When master mode, address is sent
	//	When slave mode, address matched with own address
	if(temp1 && temp3)					// for enabling ADDR flag, ITEVTEN should enabled first
	{
		//ADDR Flag is set, then clear it
		I2C_ClearADDRFlag(pI2CHandle);
	}

	temp3 	= pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_BTF);
	//3. Handle for interrupt generated by BTF(Byte Transfer Finished) event
	if(temp1 && temp3)					// for enabling BTF flag, ITEVTEN should enabled first
	{
		//BTF Flag is set
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			//make sure that TXE is also set
			if(pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE));
			{
				//BTF, TXE = 1
				if(pI2CHandle->TxLen == 0){
					//3.1. Generate stop condition
					if(pI2CHandle->Sr == DISABLE){
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
					}


					//3.2. Reset all the member elements of the handle structure
					I2C_CloseSendData();

					//3.3. Notify the application about transmission complete
					I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT);
				}
			}
		}else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			;
		}
	}

	temp3 	= pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_STOPF);
	//4. Handle for interrupt generated by STOPF event
	// Stop detection flag is applicable only on slave mode. For master this flag will never be set
	if(temp1 && temp3)
	{
		//STOPF flag is set
		//Clear the stop flag (read SR1, write to CR1)
		pI2CHandle->pI2Cx->CR1 |= 0x0000;	//write bitwise OR with 0 so none of the settings were affected

		//Notify the application about transmission complete
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_STOP);
	}

	temp3 	= pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE);
	//5. Handle for interrupt generated by TXE event
	if(temp1 && temp2 && temp3)
	{
		//Check for device mode (Master or Slave)
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{
			//TXE flag is set
			//We have to do the data transmit
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
			{
				if(pI2CHandle->TxLen > 0)
				{
					//1. Load data into DR
					pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

					//2. Decrement the TX length
					pI2CHandle->TxLen--;

					//3. Increment the buffer address
					pI2CHandle->pTxBuffer++;
				}
			}
		}
	}

	temp3 	= pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_RXNE);
	//6. Handle for interrupt generated by RXNE event
	if(temp1 && temp2 && temp3)
	{
		//check device mode
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{
			//RXNE flag is set
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
			{
				if(pI2CHandle->RxSize == 1)
				{
						//Read data from DR
						*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;

						//Decrement RX length
						pI2CHandle->RxLen--;

				}if(pI2CHandle->RxSize > 1)
				{

					if(pI2CHandle->RxLen == 2)
					{
						//clear the ack bit
						I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);
					}

					//Read data from DR
					*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;

					//Decrement RX length
					pI2CHandle->RxLen--;

					//Increment RX buffer
					pI2CHandle->pRxBuffer++;

				}
				if(pI2CHandle->RxLen == 0)
				{
					//Close the I2C
					if(pI2CHandle->Sr == DISABLE){
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
					}

					//Close the I2C receive
					I2C_CloseReceiveData(pI2CHandle);

					//Notify the application
					I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_RX_CMPLT);
				}
			}
		}
	}

}

void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);

/*
 * Other
 */
uint8_t I2C_GetFlagStatus(I2C_Regdef_t *pI2Cx, uint32_t FlagName)
{
	//refer flagname to stm32f407xx_i2c_driver.h
	if(pI2Cx->SR1 & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

void I2C_ManageAcking(I2C_Regdef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
	}
	else
	{
		 pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);
	}
}

static void I2C_GenerateStartCondition(I2C_Regdef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_START);
}

static void I2C_GenerateStopCondition(I2C_Regdef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}

static void I2C_ExecuteAddressPhaseWrite(I2C_Regdef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1; 	//shift 1 bit for R/W bit
	SlaveAddr &= ~(1);				//write 0 for W
	pI2Cx->DR = SlaveAddr;
}

static void I2C_ExecuteAddressPhaseRead(I2C_Regdef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1; 	//shift 1 bit for R/W bit
	SlaveAddr |= 1;					//write 1 for R
	pI2Cx->DR = SlaveAddr;
}

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	uint32_t dummy_read;

	//check for device mode
	if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
	{
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if(pI2CHandle->RxSize == 1)

				//disable ACK
				I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);

				//clear ADDR flag
				dummy_read = pI2CHandle->pI2Cx->SR1;
				dummy_read = pI2CHandle->pI2Cx->SR2;
				(void)dummy_read;
		}else
		{
			//clear ADDR flag
			dummy_read = pI2CHandle->pI2Cx->SR1;
			dummy_read = pI2CHandle->pI2Cx->SR2;
			(void)dummy_read;
		}
	}else
	{
	dummy_read = pI2CHandle->pI2Cx->SR1;
	dummy_read = pI2CHandle->pI2Cx->SR2;
	(void)dummy_read;
	}

}

