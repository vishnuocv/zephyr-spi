# STM32N6 SPI Communication Project

This repository contains two Zephyr applications for SPI testing between an STM32N6570-DK (Master) and a Nucleo-N6570 (Slave).

## Build Instructions

### Build Master (DK Board)
\`\`\`bash
west build -b stm32n6570_dk -d spi_master/build spi_master --sysbuild -p always
\`\`\`

### Build Slave (Nucleo Board)
\`\`\`bash
west build -b nucleo_n657x0_q -d spi_slave/build spi_slave --sysbuild -p always
\`\`\`

## Connections
- SCK: D13 <-> D13
- MISO: D12 <-> D12
- MOSI: D11 <-> D11
- CS: D10 <-> D10
- GND: Connect GND to GND
