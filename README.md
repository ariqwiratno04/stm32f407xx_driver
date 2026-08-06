# STM32F407 Bare-Metal Programming

A repository for me learning low-level, bare-metal C programming on the **STM32F407G-DISC1 (STM32F4 Discovery)** board using **STM32CubeIDE**.

This project focuses on manipulating memory-mapped registers directly using C pointers, gaining a ground-up understanding of the ARM Cortex-M4 architecture and STM32 peripherals without high-level abstraction libraries like HAL or LL.

## 🎯 Objectives & Focus Areas

- **Register-Level Control:** Writing driver code by referencing the STM32F407 Reference Manual (RM0090) and Datasheet.
- **Clock Tree Configuration:** Manually configuring the Reset and Clock Control (RCC) block, PLL, and peripheral buses (AHB/APB).
- **Custom Peripheral Drivers:** Developing bare-metal drivers for GPIO, SPI, and other peripherals (still figuring out what to add).
- **On-Chip Debugging:** Utilizing STM32CubeIDE with ST-LINK/V2 for live register inspection, breakpoints, and flash memory analysis.

## 🛠️ Hardware Requirements

* **Development Board:** STM32F4 Discovery (`STM32F407G-DISC1` / `STM32F407VG`)
  * MCU: STM32F407VGT6 (ARM Cortex-M4 with FPU, 168 MHz, 1 MB Flash, 192 KB SRAM)
  * On-board Programmer/Debugger: ST-LINK/V2
* **Peripherals / Accessories:**
  * Mini-USB cable (for ST-LINK connection and programming)
  * Logic analyzer / Oscilloscope (optional, for protocol and timing analysis)

## 💻 Software & Toolchain

* **IDE:** [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) (Eclipse-based IDE with integrated `arm-none-eabi-gcc` toolchain and ST-LINK GDB server)
* **Compiler:** `arm-none-eabi-gcc` (embedded within STM32CubeIDE)
* **Debugging Tooling:** ST-LINK GDB Server / OpenOCD (integrated in IDE)
