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

	uint8_t tx = 0x9F;
	uint8_t rx = 0x00;

	struct spi_buf tx_buf = {
		.buf = &tx,
		.len = 1,
	};

	struct spi_buf rx_buf = {
		.buf = &rx,
		.len = 1,
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

	cfg.frequency = 1000000;
	cfg.operation =
		SPI_WORD_SET(8) |
		SPI_TRANSFER_MSB |
		SPI_MODE_CPOL |
		SPI_MODE_CPHA |
		SPI_OP_MODE_MASTER;
	cfg.slave = 0;

	/* HW-controlled NSS */
	cfg.cs = (struct spi_cs_control){ 0 };

	while (1) {
		LOG_INF("---- SPI transfer ----");
		LOG_INF("TX = 0x%02X", tx);

		ret = spi_transceive(spi, &cfg, &tx_set, &rx_set);
		if (ret) {
			LOG_ERR("spi_transceive failed (%d)", ret);
		} else {
			LOG_INF("RX = 0x%02X", rx);
		}

		k_sleep(K_SECONDS(1));
	}
}

