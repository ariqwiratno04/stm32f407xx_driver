/*
 * stm32f407xx.h
 *
 *  Created on: Jul 25, 2026
 *      Author: LEGION
 */

#ifndef INC_STM32F407XX_H_
#define INC_STM32F407XX_H_

#include <stdint.h>

#define __vo volatile 			//volatile variable
/*
 * Base address of Flash and SRAM memories
 */
#define FLASH_BASEADDR			0x08000000U
#define SRAM1_BASEAADDR			0x20000000U
#define SRAM2_BASEADDR			0x2001C000U
#define ROM_BASEADDR			0x1FFF0000U


/*
 * AHBx and APBx Bus Peripheral Addresses
 */
#define PERIPH_BASE				0x40000000U
#define APB1PERIPH_BASE			PERIPH_BASE
#define	APB2PERIPH_BASE			0x40010000U
#define	AHB1PERIPH_BASE			0x40020000U
#define	AHB2PERIPH_BASE			0x50000000U

/*
 * Base address peripheral on AHB1 Bus
 */
#define GPIOA_BASEADDR			(AHB1PERIPH_BASE + 0x0000U)
#define GPIOB_BASEADDR			(AHB1PERIPH_BASE + 0x0400U)
#define GPIOC_BASEADDR			(AHB1PERIPH_BASE + 0x0800U)
#define GPIOD_BASEADDR			(AHB1PERIPH_BASE + 0x0C00U)
#define GPIOE_BASEADDR			(AHB1PERIPH_BASE + 0x1000U)
#define GPIOF_BASEADDR			(AHB1PERIPH_BASE + 0x1400U)
#define GPIOG_BASEADDR			(AHB1PERIPH_BASE + 0x1800U)
#define GPIOH_BASEADDR			(AHB1PERIPH_BASE + 0x1C00U)
#define GPIOI_BASEADDR			(AHB1PERIPH_BASE + 0x2000U)
#define GPIOJ_BASEADDR			(AHB1PERIPH_BASE + 0x2400U)
#define GPIOK_BASEADDR			(AHB1PERIPH_BASE + 0x2800U)

#define RCC_BASEADDR			(AHB1PERIPH_BASE + 0x3800U)

/*
 * Base address peripheral on APB1 Bus
 */
#define I2C1_BASEADDR			(APB1PERIPH_BASE + 0x5400U)
#define	I2C2_BASEADDR			(APB1PERIPH_BASE + 0x5800U)
#define I2C3_BASEADDR			(APB1PERIPH_BASE + 0x5C00U)
#define	SPI2_BASEADDR			(APB1PERIPH_BASE + 0x3800U)
#define SPI3_BASEADDR			(APB1PERIPH_BASE + 0x3C00U)
#define USART2_BASEADDR			(APB1PERIPH_BASE + 0x4400U)
#define USART3_BASEADDR			(APB1PERIPH_BASE + 0x4800U)
#define UART4_BASEADDR			(APB1PERIPH_BASE + 0x4C00U)
#define UART5_BASEADDR			(APB1PERIPH_BASE + 0x5000U)

/*
 * Base address peripheral on APB2 Bus
 */
#define USART1_BASEADDR			(APB2PERIPH_BASE + 0x1000U)
#define USART6_BASEADDR			(APB2PERIPH_BASE + 0x1400U)
#define	SPI1_BASEADDR			(APB2PERIPH_BASE + 0x3000U)
#define SPI4_BASEADDR			(APB2PERIPH_BASE + 0x3400U)
#define SYSCFG_BASEADDR			(APB2PERIPH_BASE + 0x3800U)
#define EXTI_BASEADDR			(APB2PERIPH_BASE + 0x3C00U)



/*
 * Struct for various peripheral
 */
typedef struct{

	__vo uint32_t MODER;			//offset 0x00
	__vo uint32_t OTYPER;			//offset 0x04
	__vo uint32_t OSPEEDR;			//offset 0x08
	__vo uint32_t PUPDR;			//offset 0x0C
	__vo uint32_t IDR;				//offset 0x10
	__vo uint32_t ODR;				//offset 0x14
	__vo uint32_t BSSR;				//offset 0x18
	__vo uint32_t LCKR;				//offset 0x1C
	__vo uint32_t AFRL;				//offset 0x20
	__vo uint32_t AFRH;				//offset 0x24

}GPIO_Regdef_t;

typedef struct{

	__vo uint32_t CR;				//offset 0x00
	__vo uint32_t PLLCFGR;			//offset 0x04
	__vo uint32_t CFGR;				//offset 0x08
	__vo uint32_t CIR;				//offset 0x0C
	__vo uint32_t AHB1RSTR;			//offset 0x10
	__vo uint32_t AHB2RSTR;			//offset 0x14
	__vo uint32_t AHB3RSTR;			//offset 0x18
	__vo uint32_t RESERVED0;		//offset 0x1C
	__vo uint32_t APB1RSTR;			//offset 0x20
	__vo uint32_t APB2RSTR;			//offset 0x24
	__vo uint32_t RESERVED1;		//offset 0x28
	__vo uint32_t RESERVED2;		//offset 0x2C
	__vo uint32_t AHB1ENR;			//offset 0x30
	__vo uint32_t AHB2ENR;			//offset 0x34
	__vo uint32_t AHB3ENR;			//offset 0x38
	__vo uint32_t RESERVED3;		//offset 0x3C
	__vo uint32_t APB1ENR;			//offset 0x40
	__vo uint32_t APB2ENR;			//offset 0x44
	__vo uint32_t RESERVED4;		//offset 0x48
	__vo uint32_t RESERVED5;		//offset 0x4C
	__vo uint32_t AHB1LPENR;		//offset 0x50
	__vo uint32_t AHB2LPENR;		//offset 0x54
	__vo uint32_t AHB3LPENR;		//offset 0x58
	__vo uint32_t RESERVED6;		//offset 0x5C
	__vo uint32_t APB1LPENR;		//offset 0x60
	__vo uint32_t APB2LPENR;		//offset 0x64
	__vo uint32_t RESERVED7;		//offset 0x68
	__vo uint32_t RESERVED8;		//offset 0x6C
	__vo uint32_t BDCR;				//offset 0x70
	__vo uint32_t CSR;				//offset 0x74
	__vo uint32_t RESERVED9;		//offset 0x78
	__vo uint32_t RESERVED10;		//offset 0x7C
	__vo uint32_t SSCGR;			//offset 0x80
	__vo uint32_t PLLI2SCFGR;		//offset 0x84
	__vo uint32_t PLLSAICFGR;		//offset 0x88
	__vo uint32_t DCKCFGR;			//offset 0x8C

}RCC_Regdef_t;

/*
 * Peripheral defintions base addresses typecasted to xxx_Regdef_t
 */

#define GPIOA					((GPIO_Regdef_t*)GPIOA_BASEADDR)
#define GPIOB					((GPIO_Regdef_t*)GPIOB_BASEADDR)
#define GPIOC					((GPIO_Regdef_t*)GPIOC_BASEADDR)
#define GPIOD					((GPIO_Regdef_t*)GPIOD_BASEADDR)
#define GPIOE					((GPIO_Regdef_t*)GPIOE_BASEADDR)
#define GPIOF					((GPIO_Regdef_t*)GPIOF_BASEADDR)
#define GPIOG					((GPIO_Regdef_t*)GPIOG_BASEADDR)
#define GPIOH					((GPIO_Regdef_t*)GPIOH_BASEADDR)
#define GPIOI					((GPIO_Regdef_t*)GPIOI_BASEADDR)

#define RCC						((RCC_Regdef_t*)RCC_BASEADDR)

/*
 * Clock enable and disable macros for GPIOx peripherals
 */
#define GPIOA_PCLK_EN()			(RCC->AHB1ENR |= (1 << 0))
#define GPIOA_PCLK_DI()			(RCC->AHB1ENR &= ~(1 << 0))

#define GPIOB_PCLK_EN()			(RCC->AHB1ENR |= (1 << 1))
#define GPIOB_PCLK_DI()			(RCC->AHB1ENR &= ~(1 << 1))

#define GPIOC_PCLK_EN()			(RCC->AHB1ENR |= (1 << 2))
#define GPIOC_PCLK_DI()			(RCC->AHB1ENR &= ~(1 << 2))

#define GPIOD_PCLK_EN()			(RCC->AHB1ENR |= (1 << 3))
#define GPIOD_PCLK_DI()			(RCC->AHB1ENR &= ~(1 << 3))

#define GPIOE_PCLK_EN()			(RCC->AHB1ENR |= (1 << 4))
#define GPIOE_PCLK_DI()			(RCC->AHB1ENR &= ~(1 << 4))

#define GPIOF_PCLK_EN()			(RCC->AHB1ENR |= (1 << 5))
#define GPIOF_PCLK_DI()			(RCC->AHB1ENR &= ~(1 << 5))

#define GPIOG_PCLK_EN()			(RCC->AHB1ENR |= (1 << 6))
#define GPIOG_PCLK_DI()			(RCC->AHB1ENR &= ~(1 << 6))

#define GPIOH_PCLK_EN()			(RCC->AHB1ENR |= (1 << 7))
#define GPIOH_PCLK_DI()			(RCC->AHB1ENR &= ~(1 << 7))

#define GPIOI_PCLK_EN()			(RCC->AHB1ENR |= (1 << 8))
#define GPIOI_PCLK_DI()			(RCC->AHB1ENR &= ~(1 << 8))


/*
 * Clock reset macros for GPIOx peripherals
 */
#define GPIOA_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); } while(0)
#define GPIOB_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 1)); (RCC->AHB1RSTR &= ~(1 << 1)); } while(0)
#define GPIOC_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 2)); (RCC->AHB1RSTR &= ~(1 << 2)); } while(0)
#define GPIOD_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 3)); (RCC->AHB1RSTR &= ~(1 << 3)); } while(0)
#define GPIOE_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 4)); (RCC->AHB1RSTR &= ~(1 << 4)); } while(0)
#define GPIOF_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 5)); (RCC->AHB1RSTR &= ~(1 << 5)); } while(0)
#define GPIOG_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 6)); (RCC->AHB1RSTR &= ~(1 << 6)); } while(0)
#define GPIOH_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 7)); (RCC->AHB1RSTR &= ~(1 << 7)); } while(0)
#define GPIOI_REG_RESET()		do{ (RCC->AHB1RSTR |= (1 << 8)); (RCC->AHB1RSTR &= ~(1 << 8)); } while(0)

/*
 * Clock enable and disable macros for I2Cx peripherals
 */
#define I2C1_PCLK_EN()			(RCC->APB1ENR |= (1 << 21))
#define I2C1_PCLK_DI()			(RCC->APB1ENR &= ~(1 << 21))

/*
 * Clock enable and disable macros for SPIx peripherals
 */
#define SPI1_PCLK_EN()			(RCC->APB2ENR |= (1 << 12))
#define SPI1_PCLK_DI()			(RCC->APB2ENR &= ~(1 << 12))

/*
 * Clock enable and disable macros for USARTx peripherals
 */
#define USART1_PCLK_EN()		(RCC->APB2ENR |= (1 << 4))
#define USART1_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 4))

/*
 * Clock enable and disable macros for SYSCFG peripherals
 */
#define SYSCFG_PCLK_EN()		(RCC->APB2ENR |= (1 << 14))
#define SYSCFG_PCLK_DI()		(RCC->APB2ENR &= ~(1 << 14))


/*
 * General purpose macros
 */
#define ENABLE					1
#define DISABLE					0
#define SET						ENABLE
#define RESET					DISABLE
#define GPIO_PIN_SET			SET
#define GPIO_PIN_RESET			RESET

#include "stm32f407xx_gpio_driver.h"

#endif /* INC_STM32F407XX_H_ */
