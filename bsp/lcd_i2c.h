/*
 * lcd_i2c.h
 *
 *  Created on: Oct 6, 2026
 *      Author: EE-11
 */

#ifndef LCD_I2C_H_
#define LCD_I2C_H_

#include "stm32f407xx.h"


#define LCD_I2C_ADDR        0x27

#define LCD_I2C				I2C2
#define LCD_I2C_GPIO_PORT	GPIOB
#define LCD_I2C_GPIO_SCL	GPIO_PIN_NO_10
#define LCD_I2C_GPIO_SDA	GPIO_PIN_NO_11
#define LCD_I2C_SPEED		I2C_SCL_SPEED_SM
#define LCD_I2C_PUPD		GPIO_NO_PUPD


/*
 * Exposed APIs
 */
void lcd_init(void);
void lcd_send_command(uint8_t cmd);
void lcd_print_char(uint8_t data);
void lcd_display_clear(void);
void lcd_display_return_home(void);
void lcd_print_string(char*);
void lcd_set_cursor(uint8_t row, uint8_t column);

/*
 * Internal APIs
 */
void lcd_i2c_config_init(void);
void lcd_i2c_gpio_init(void);
void lcd_send_command(uint8_t cmd);

/*LCD commands */
#define LCD_CMD_4DL_2N_5X8F  		0x28
#define LCD_CMD_DON_CURON    		0x0E
#define LCD_CMD_INCADD       		0x06
#define LCD_CMD_DIS_CLEAR    		0X01
#define LCD_CMD_DIS_RETURN_HOME  	0x02

#endif /* LCD_I2C_H_ */
