/*
 * lcd_i2c.c
 *
 *  Created on: Oct 6, 2026
 *      Author: EE-11
 */

#include "stm32f407xx.h"
#include "lcd_i2c.h"

I2C_Handle_t lcdi2chandle;

static void write_8bit(uint8_t data);
static void write_4bit_data(uint8_t data);
static void BL_bit(uint8_t EnorDi);
static void RW_bit(uint8_t EnorDi);
static void RS_bit(uint8_t EnorDi);
static void lcd_enable();
static void mdelay(uint32_t cnt);
static void udelay(uint32_t cnt);


void lcd_send_command(uint8_t cmd)
{
	/* RS=0 for LCD command */
	RS_bit(DISABLE);

	/*R/nW = 0, for write */
	RW_bit(DISABLE);

	write_4bit_data(cmd >> 4);
	write_4bit_data(cmd & 0x0F);

}

/*
 *This function sends a character to the LCD
 *Here we used 4 bit parallel data transmission.
 *First higher nibble of the data will be sent on to the data lines D4,D5,D6,D7
 *Then lower nibble of the data will be set on to the data lines D4,D5,D6,D7
 */
void lcd_print_char(uint8_t data)
{
	/* RS=1 for LCD user data */
	RS_bit(ENABLE);

	/*R/nW = 0, for write */
	RW_bit(DISABLE);

	write_4bit_data(data >> 4);  /*Higher nibble*/
	write_4bit_data(data & 0x0F); /*Lower nibble*/

	//BL_bit(ENABLE);
}


void lcd_print_string(char *message)
{

      do
      {
          lcd_print_char((uint8_t)*message++);
      }
      while (*message != '\0');

}

void lcd_i2c_config_init(void)
{
	lcdi2chandle.pI2Cx							= LCD_I2C;
	lcdi2chandle.I2C_Config.I2C_ACKControl		= I2C_ACK_ENABLE;
	lcdi2chandle.I2C_Config.I2C_SCLSpeed		= LCD_I2C_SPEED;

	I2C_Init(&lcdi2chandle);
}

void lcd_i2c_gpio_init(void)
{
	GPIO_Handle_t lcdi2c_scl, lcdi2c_sda;

	lcdi2c_scl.pGPIOx = LCD_I2C_GPIO_PORT;
	lcdi2c_scl.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_AF4;
	lcdi2c_scl.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALT;
	lcdi2c_scl.GPIO_PinConfig.GPIO_PinNumber = LCD_I2C_GPIO_SCL;
	lcdi2c_scl.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	lcdi2c_scl.GPIO_PinConfig.GPIO_PinPuPdControl = LCD_I2C_PUPD;
	lcdi2c_scl.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

	GPIO_Init(&lcdi2c_scl);

	lcdi2c_sda.pGPIOx = LCD_I2C_GPIO_PORT;
	lcdi2c_sda.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_AF4;
	lcdi2c_sda.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALT;
	lcdi2c_sda.GPIO_PinConfig.GPIO_PinNumber = LCD_I2C_GPIO_SDA;
	lcdi2c_sda.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	lcdi2c_sda.GPIO_PinConfig.GPIO_PinPuPdControl = LCD_I2C_PUPD;
	lcdi2c_sda.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

	GPIO_Init(&lcdi2c_sda);
}

void lcd_init(void)
{
	lcd_i2c_config_init();
	lcd_i2c_gpio_init();

	I2C_PeripheralControl(LCD_I2C, ENABLE);

	write_8bit(0x8);

	//LCD initialization
	mdelay(40);

	RW_bit(DISABLE);
	RS_bit(DISABLE);
	write_4bit_data(0x3);

	mdelay(5);

	write_4bit_data(0x3);

	udelay(120);

	write_4bit_data(0x3);
	write_4bit_data(0x2);

	//function set command
	lcd_send_command(LCD_CMD_4DL_2N_5X8F);

	//disply ON and cursor ON
	lcd_send_command(LCD_CMD_DON_CURON);

	lcd_display_clear();

	//entry mode set
	lcd_send_command(LCD_CMD_INCADD);

	//BL_bit(ENABLE);

}

static void write_4bit_data(uint8_t data)
{
	//fetch the current pin config
	uint8_t previous_data;
	I2C_MasterReceiveData(&lcdi2chandle, &previous_data, 1,  LCD_I2C_ADDR, 0);

	uint8_t data_bit = (uint8_t)((previous_data & 0x0B) | ((data & 0x0F) << 4));
	I2C_MasterSendData(&lcdi2chandle, &data_bit, 1, LCD_I2C_ADDR, 1);
	lcd_enable();

}

static void write_8bit(uint8_t data)
{
	I2C_MasterSendData(&lcdi2chandle, &data, 1,  LCD_I2C_ADDR, 0);
}

static void lcd_enable()
{
	//fetch the current pin config
	uint8_t previous_data;
	I2C_MasterReceiveData(&lcdi2chandle, &previous_data, 1,  LCD_I2C_ADDR, 0);

	uint8_t enable = previous_data | 0x4;
	uint8_t disable = previous_data & (uint8_t)~0x04U;

	I2C_MasterSendData(&lcdi2chandle, &enable, 1, LCD_I2C_ADDR, 0);
	udelay(10);
	I2C_MasterSendData(&lcdi2chandle, &disable, 1, LCD_I2C_ADDR, 0);
	udelay(100); //execution time > 37 micro seconds
}

void lcd_display_clear(void)
{
	//Display clear
	lcd_send_command(LCD_CMD_DIS_CLEAR);

	/*
	 * check page number 24 of datasheet.
	 * display clear command execution wait time is around 2ms
	 */

	mdelay(2);
}

/*Cursor returns to home position */
void lcd_display_return_home(void)
{

	lcd_send_command(LCD_CMD_DIS_RETURN_HOME);
	/*
	 * check page number 24 of datasheet.
	 * return home command execution wait time is around 2ms
	 */
	mdelay(2);
}

/**
  *   Set Lcd to a specified location given by row and column information
  *   Row Number (1 to 2)
  *   Column Number (1 to 16) Assuming a 2 X 16 characters display
  */
void lcd_set_cursor(uint8_t row, uint8_t column)
{
  column--;
  switch (row)
  {
    case 1:
      /* Set cursor to 1st row address and add index*/
      lcd_send_command((column |= 0x80));
      break;
    case 2:
      /* Set cursor to 2nd row address and add index*/
        lcd_send_command((column |= 0xC0));
      break;
    case 3:
      /* Set cursor to 2nd row address and add index*/
    	lcd_send_command((column |= 0xE0));
      break;
    default:
      break;
  }
}

static void RS_bit(uint8_t EnorDi)
{
	//fetch the current pin config
	uint8_t previous_data;
	I2C_MasterReceiveData(&lcdi2chandle, &previous_data, 1,  LCD_I2C_ADDR, 0);


	uint8_t enable = 0x1 | previous_data;
	uint8_t disable = previous_data & (uint8_t)~0x01U;

	if(EnorDi == ENABLE)
	{
		I2C_MasterSendData(&lcdi2chandle, &enable, 1, LCD_I2C_ADDR, 0);
	}else
	{
		I2C_MasterSendData(&lcdi2chandle, &disable, 1, LCD_I2C_ADDR, 0);
	}
}

static void BL_bit(uint8_t EnorDi)
{
	//fetch the current pin config
	uint8_t previous_data;
	I2C_MasterReceiveData(&lcdi2chandle, &previous_data, 1,  LCD_I2C_ADDR, 0);


	uint8_t enable = 0x8 | previous_data;
	uint8_t disable = previous_data & (uint8_t)~0x08U;

	if(EnorDi == ENABLE)
	{
		I2C_MasterSendData(&lcdi2chandle, &enable, 1, LCD_I2C_ADDR, 0);
	}else
	{
		I2C_MasterSendData(&lcdi2chandle, &disable, 1, LCD_I2C_ADDR, 0);
	}
}

static void RW_bit(uint8_t EnorDi)
{
		//fetch the current pin config
	uint8_t previous_data;
	I2C_MasterReceiveData(&lcdi2chandle, &previous_data, 1,  LCD_I2C_ADDR, 0);

	uint8_t enable = 0x2 | previous_data;
	uint8_t disable = previous_data & (uint8_t)~0x02U;

	if(EnorDi == ENABLE)
	{
		I2C_MasterSendData(&lcdi2chandle, &enable, 1, LCD_I2C_ADDR, 0);
	}else
	{
		I2C_MasterSendData(&lcdi2chandle, &disable, 1, LCD_I2C_ADDR, 0);
	}
}

static void mdelay(uint32_t cnt)
{
	for(uint32_t i=0 ; i < (cnt * 1000); i++);
}

static void udelay(uint32_t cnt)
{
	for(uint32_t i=0 ; i < (cnt * 1); i++);
}
