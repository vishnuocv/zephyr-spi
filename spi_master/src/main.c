#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(spi_test, LOG_LEVEL_DBG);
#define SPI_NODE DT_NODELABEL(spi5)   

int main(void)
{
	const struct device *spi;
	struct spi_config cfg;
	int ret;
/*
	uint8_t tx = 0x9F;
	uint8_t rx = 0x00;
*/
	uint8_t tx[8] = {0x9F, 0xAA, 0x44, 0x11, 0x22, 0x33, 0x66, 0x55};
	uint8_t rx[8] = {0};

	struct spi_buf tx_buf = {
		.buf = &tx,
		.len = sizeof(tx),
	};

	struct spi_buf rx_buf = {
		.buf = &rx,
		.len = sizeof(rx),
	};

	struct spi_buf_set tx_set = {
		.buffers = &tx_buf,
		.count = 1,
	};

	struct spi_buf_set rx_set = {
		.buffers = &rx_buf,
		.count = 1,
	};

	LOG_INF("SPI bring-up test");

	/* Correct way on STM32N6 */
	spi = DEVICE_DT_GET_ONE(st_stm32_spi);
	if (!device_is_ready(spi)) {
		LOG_ERR("SPI device not ready");
		return 0;
	}


#if 0
        spi = DEVICE_DT_GET(SPI_NODE);

        printk("SPI Master Device check\n");

        if (!device_is_ready(spi)) {
                printk("SPI device not ready\n");
                return;
        }
#endif

	LOG_INF("Using SPI device: %s", spi->name);

	cfg.frequency = 250000;
	cfg.operation =
		SPI_WORD_SET(8) |
		SPI_TRANSFER_MSB |
		SPI_OP_MODE_MASTER;
	cfg.slave = 0;

	/* HW-controlled NSS */
	cfg.cs = (struct spi_cs_control){ 0 };

	/* Use GPIO CS - more reliable */
/*	cfg.cs = (struct spi_cs_control){
		.gpio = GPIO_DT_SPEC_GET(DT_NODELABEL(spi5), cs_gpios),
		.delay = 100,
	};
*/
	while (1) {
		LOG_INF("---- SPI transfer ----");
//		LOG_INF("TX = 0x%02X", tx);
		LOG_INF("TX: %02X %02X %02X %02X %02X %02X %02X %02X", 
			tx[0], tx[1], tx[2], tx[3],
			tx[4], tx[5], tx[6], tx[7]);

		ret = spi_transceive(spi, &cfg, &tx_set, &rx_set);
		if (ret) {
			LOG_ERR("spi_transceive failed (%d)", ret);
		} else {
//			LOG_INF("RX = 0x%02X", rx);
			LOG_INF("RX: %02X %02X %02X %02X %02X %02X %02X %02X",
				rx[0], rx[1], rx[2], rx[3],
				rx[4], rx[5], rx[6], rx[7]);
		}

		k_sleep(K_SECONDS(1));
	}
}

