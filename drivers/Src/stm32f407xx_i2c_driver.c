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

	//Config the ACK control bit
	pI2CHandle->pI2Cx->CR1 |= pI2CHandle->I2C_Config.I2C_ACKControl << I2C_CR1_ACK;

	//Config FREQ register CR2
	uint32_t tempreg = 0;
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
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr)
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
	I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
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

/*
 * Other
 */
uint8_t I2C_GetFlagStatus(I2C_Regdef_t *pI2Cx, uint32_t FlagName)
{
	if(pI2Cx->SR1 & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
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

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	uint32_t dummy_read;
	dummy_read = pI2CHandle->pI2Cx->SR1;
	dummy_read = pI2CHandle->pI2Cx->SR2;
	(void)dummy_read;

}

