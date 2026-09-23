/*
 * Non-blocking interrupt-driven I2C master example.
 *
 * Connections:
 *   PB6  -> I2C1_SCL
 *   PB7  -> I2C1_SDA
 *   PA0  -> user button (active high, rising-edge interrupt)
 *   PD12 -> green LED
 *
 * Arduino slave protocol:
 *   write 0x51, repeated START, read one-byte message length
 *   write 0x52, repeated START, read message, STOP
 *
 * Only one source file containing main() should be enabled in the project.
 */

#include "stm32f407xx.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MY_ADDRESS             0x61U
#define SLAVE_ADDRESS          0x68U
#define COMMAND_GET_LENGTH     0x51U
#define COMMAND_GET_DATA       0x52U

#define RX_BUFFER_SIZE         32U
#define LED_BLINK_PERIOD_MS    250U
#define BUTTON_DEBOUNCE_MS     50U

/* Cortex-M4 SysTick registers. */
#define SYSTICK_CTRL           (*(volatile uint32_t *)0xE000E010U)
#define SYSTICK_LOAD           (*(volatile uint32_t *)0xE000E014U)
#define SYSTICK_VALUE          (*(volatile uint32_t *)0xE000E018U)

#define SYSTICK_CTRL_ENABLE    (1U << 0)
#define SYSTICK_CTRL_TICKINT   (1U << 1)
#define SYSTICK_CTRL_CLKSOURCE (1U << 2)

#define APP_EVENT_TX_COMPLETE  (1U << 0)
#define APP_EVENT_RX_COMPLETE  (1U << 1)
#define APP_EVENT_I2C_ERROR    (1U << 2)

typedef enum
{
	APP_STATE_IDLE = 0,
	APP_STATE_WAIT_LENGTH_COMMAND_TX,
	APP_STATE_WAIT_LENGTH_RX,
	APP_STATE_WAIT_DATA_COMMAND_TX,
	APP_STATE_WAIT_DATA_RX
} AppState_t;

static I2C_Handle_t I2C1Handle;
static uint8_t rxBuffer[RX_BUFFER_SIZE];
static uint8_t commandCode;
static uint8_t messageLength;

/* Variables shared with interrupt handlers must be volatile. */
static volatile AppState_t appState = APP_STATE_IDLE;
static volatile uint32_t appEvents;
static volatile uint8_t buttonPressed;
static volatile uint8_t i2cErrorCode;
static volatile uint8_t messageReady;
static volatile uint32_t milliseconds;

static void I2C1_GPIOInits(void)
{
	GPIO_Handle_t pins;

	memset(&pins, 0, sizeof(pins));
	pins.pGPIOx = GPIOB;
	pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALT;
	pins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_AF4;
	pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

	pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&pins);

	pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&pins);
}

static void I2C1_Inits(void)
{
	memset(&I2C1Handle, 0, sizeof(I2C1Handle));
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDRESS;

	I2C_Init(&I2C1Handle);
}

static void Button_Inits(void)
{
	GPIO_Handle_t button;

	memset(&button, 0, sizeof(button));
	button.pGPIOx = GPIOA;
	button.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	button.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RT;
	button.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	button.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&button);
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI0, 15U);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI0, ENABLE);
}

static void GreenLed_Inits(void)
{
	GPIO_Handle_t led;

	memset(&led, 0, sizeof(led));
	led.pGPIOx = GPIOD;
	led.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	led.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&led);
	GPIO_WriteToOutputPin(GPIOD, GPIO_PIN_NO_12, RESET);
}

static uint32_t GetHclkFrequency(void)
{
	static const uint16_t ahbPrescaler[8] =
	{
		2U, 4U, 8U, 16U, 64U, 128U, 256U, 512U
	};
	uint32_t systemClock;
	uint32_t hpre;
	uint32_t clockSource = (RCC->CFGR >> 2U) & 0x3U;

	if (clockSource == 0U)
	{
		systemClock = 16000000U; /* HSI */
	}
	else if (clockSource == 1U)
	{
		systemClock = 8000000U;  /* HSE on STM32F407 Discovery */
	}
	else
	{
		systemClock = RCC_GetPLLOutputClock();
	}

	hpre = (RCC->CFGR >> 4U) & 0xFU;
	if (hpre >= 8U)
	{
		systemClock /= ahbPrescaler[hpre - 8U];
	}

	return systemClock;
}

static void SysTick_Inits(void)
{
	uint32_t hclk = GetHclkFrequency();

	SYSTICK_LOAD = (hclk / 1000U) - 1U;
	SYSTICK_VALUE = 0U;
	SYSTICK_CTRL = SYSTICK_CTRL_CLKSOURCE |
	               SYSTICK_CTRL_TICKINT |
	               SYSTICK_CTRL_ENABLE;
}

static void ServiceGreenLed(void)
{
	static uint32_t previousToggle;
	uint32_t now = milliseconds;

	if ((uint32_t)(now - previousToggle) >= LED_BLINK_PERIOD_MS)
	{
		previousToggle = now;
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
	}
}

static uint8_t StartTransmit(uint8_t *data, uint32_t length, uint8_t repeatedStart)
{
	return (I2C_MasterSendDataIT(&I2C1Handle,
	                            data,
	                            length,
	                            SLAVE_ADDRESS,
	                            repeatedStart) == I2C_READY);
}

static uint8_t StartReceive(uint8_t *data, uint32_t length, uint8_t repeatedStart)
{
	return (I2C_MasterReceiveDataIT(&I2C1Handle,
	                               data,
	                               length,
	                               SLAVE_ADDRESS,
	                               repeatedStart) == I2C_READY);
}

static void StartMessageRequest(void)
{
	messageReady = RESET;
	i2cErrorCode = 0U;
	commandCode = COMMAND_GET_LENGTH;
	appState = APP_STATE_WAIT_LENGTH_COMMAND_TX;

	if (!StartTransmit(&commandCode, 1U, I2C_ENABLE_SR))
	{
		appState = APP_STATE_IDLE;
	}
}

static void AbortRequest(uint8_t errorCode)
{
	/* A repeated-start transaction may still own the bus at this point. */
	if (I2C1->SR2 & (1U << I2C_SR2_MSL))
	{
		I2C_GenerateStopCondition(I2C1);
	}

	i2cErrorCode = errorCode;
	appState = APP_STATE_IDLE;
}

static void ProcessI2CEvents(void)
{
	uint32_t events = appEvents;

	if (events == 0U)
	{
		return;
	}

	/* A new transfer is not started until after the current event is cleared. */
	appEvents &= ~events;

	if (events & APP_EVENT_I2C_ERROR)
	{
		AbortRequest(i2cErrorCode);
		printf("I2C error: %u\n", i2cErrorCode);
		return;
	}

	if ((events & APP_EVENT_TX_COMPLETE) != 0U)
	{
		if (appState == APP_STATE_WAIT_LENGTH_COMMAND_TX)
		{
			appState = APP_STATE_WAIT_LENGTH_RX;
			if (!StartReceive(&messageLength, 1U, I2C_ENABLE_SR))
			{
				AbortRequest(0xF1U);
			}
		}
		else if (appState == APP_STATE_WAIT_DATA_COMMAND_TX)
		{
			appState = APP_STATE_WAIT_DATA_RX;
			if (!StartReceive(rxBuffer,
			                  messageLength,
			                  I2C_DISABLE_SR))
			{
				AbortRequest(0xF2U);
			}
		}
	}

	if ((events & APP_EVENT_RX_COMPLETE) != 0U)
	{
		if (appState == APP_STATE_WAIT_LENGTH_RX)
		{
			/* Leave one byte free for the C string terminator. */
			if ((messageLength == 0U) || (messageLength >= RX_BUFFER_SIZE))
			{
				AbortRequest(0xF3U);
				return;
			}

			commandCode = COMMAND_GET_DATA;
			appState = APP_STATE_WAIT_DATA_COMMAND_TX;
			if (!StartTransmit(&commandCode, 1U, I2C_ENABLE_SR))
			{
				AbortRequest(0xF4U);
			}
		}
		else if (appState == APP_STATE_WAIT_DATA_RX)
		{
			rxBuffer[messageLength] = '\0';
			messageReady = SET;
			appState = APP_STATE_IDLE;
			printf("Data: %s\n", rxBuffer);
		}
	}
}

int main(void)
{
	GreenLed_Inits();
	Button_Inits();
	I2C1_GPIOInits();
	I2C1_Inits();
	SysTick_Inits();

	I2C_IRQPriorityConfig(IRQ_I2C1_EV, 1U);
	I2C_IRQPriorityConfig(IRQ_I2C1_ER, 1U);
	I2C_IRQInterruptConfig(IRQ_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_I2C1_ER, ENABLE);

	I2C_PeripheralControl(I2C1, ENABLE);
	I2C_ManageAcking(I2C1, ENABLE);

	printf("Non-blocking I2C example ready\n");

	while (1)
	{
		ServiceGreenLed();

		if (buttonPressed != 0U)
		{
			buttonPressed = 0U;
			if (appState == APP_STATE_IDLE)
			{
				StartMessageRequest();
			}
		}

		ProcessI2CEvents();
	}
}

void SysTick_Handler(void)
{
	milliseconds++;
}

void EXTI0_IRQHandler(void)
{
	static uint32_t previousPress = (uint32_t)(0U - BUTTON_DEBOUNCE_MS);
	uint32_t now;

	GPIO_IRQHandling(GPIO_PIN_NO_0);
	now = milliseconds;

	/* Ignore switch bounce and additional presses while a request is active. */
	if (((uint32_t)(now - previousPress) >= BUTTON_DEBOUNCE_MS) &&
	    (appState == APP_STATE_IDLE))
	{
		previousPress = now;
		buttonPressed = 1U;
	}
}

void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&I2C1Handle);
}

void I2C1_ER_IRQHandler(void)
{
	I2C_ER_IRQHandling(&I2C1Handle);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *handle, uint8_t event)
{
	if (event == I2C_EV_TX_CMPLT)
	{
		appEvents |= APP_EVENT_TX_COMPLETE;
	}
	else if (event == I2C_EV_RX_CMPLT)
	{
		appEvents |= APP_EVENT_RX_COMPLETE;
	}
	else if ((event >= I2C_ERROR_BERR) && (event <= I2C_ERROR_TIMEOUT))
	{
		/* Restore the driver's READY state so another press can retry. */
		if (handle->TxRxState == I2C_BUSY_IN_TX)
		{
			I2C_CloseSendData(handle);
		}
		else if (handle->TxRxState == I2C_BUSY_IN_RX)
		{
			I2C_CloseReceiveData(handle);
		}

		if (handle->pI2Cx->SR2 & (1U << I2C_SR2_MSL))
		{
			I2C_GenerateStopCondition(handle->pI2Cx);
		}

		i2cErrorCode = event;
		appEvents |= APP_EVENT_I2C_ERROR;
	}
}
