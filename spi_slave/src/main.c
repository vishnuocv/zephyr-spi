#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <string.h>

LOG_MODULE_REGISTER(spi_slave, LOG_LEVEL_DBG);

#define SPI_NODE DT_NODELABEL(spi5)

int main(void)
{
	const struct device *spi_dev;
	int ret;

	/* Transmit buffer - data to send to master */
	uint8_t tx_buf[8] = {0xAA, 0x55, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    
	/* Receive buffer - data received from master */
	uint8_t rx_buf[8] = {0};

	/* SPI buffers */
	struct spi_buf tx = {
		.buf = tx_buf,
		.len = sizeof(tx_buf),
	};

	struct spi_buf rx = {
		.buf = rx_buf,
		.len = sizeof(rx_buf),
	};

	/* SPI buffer sets */
	struct spi_buf_set tx_set = {
		.buffers = &tx,
		.count = 1,
	};

	struct spi_buf_set rx_set = {
		.buffers = &rx,
		.count = 1,
	};

	/* 
	 * SPI configuration for SLAVE mode
	 * IMPORTANT: No .frequency field for slave!
	 * Operation mode must match the master:
	 * - Mode 0: No CPOL, No CPHA
	 * - Mode 3: CPOL | CPHA (as shown below)
	 */
	
	struct spi_config cfg = {
//	        .frequency = 250000,        /* MUST set a valid frequency for STM32 driver */
		.operation = SPI_OP_MODE_SLAVE |      /* Slave mode */
			SPI_WORD_SET(8) |         /* 8-bit data */
			SPI_TRANSFER_MSB |        /* MSB first */
			SPI_MODE_CPOL |           /* Clock polarity (1 for Mode 3) */
			SPI_MODE_CPHA,            /* Clock phase (1 for Mode 3) */
			.slave = 0,                           /* Slave index 0 */
		/* No .frequency field - not used in slave mode */
	};

	/* Get SPI device */
	spi_dev = DEVICE_DT_GET(SPI_NODE);

	if (!device_is_ready(spi_dev)) {
		LOG_ERR("SPI device %s not ready", spi_dev->name);
		return -ENODEV;
	}

	LOG_INF("SPI slave initialized on %s", spi_dev->name);
	LOG_INF("Waiting for SPI master...");
	LOG_INF("TX data: %02X %02X %02X %02X %02X %02X %02X %02X", 
		tx_buf[0], tx_buf[1], tx_buf[2], tx_buf[3],
		tx_buf[4], tx_buf[5], tx_buf[6], tx_buf[7]);

	while (1) {
		/* Clear receive buffer */
		memset(rx_buf, 0, sizeof(rx_buf));

		/* 
	 	 * Wait for SPI transaction from master
	 	 * This call blocks until CS is asserted and clock is provided
		 */
		ret = spi_transceive(spi_dev, &cfg, &tx_set, &rx_set);

		if (ret == 0) {
			/* Successful transaction */
			LOG_INF("SPI transaction complete");
			LOG_INF("RX data: %02X %02X %02X %02X %02X %02X %02X %02X", 
				rx_buf[0], rx_buf[1], rx_buf[2], rx_buf[3],
				rx_buf[4], rx_buf[5], rx_buf[6], rx_buf[7]);

			/* Optional: Update TX buffer for next transaction */
			tx_buf[0]++;  /* Increment first byte as example */

		} else if (ret == -EIO) {
			/* Common error if CS is deasserted prematurely */
			LOG_WRN("SPI transaction incomplete (CS deasserted?)");
		} else {
			/* Other errors */
			LOG_ERR("SPI transceive failed: %d", ret);
		}

		/* Small delay to prevent tight loop */
		k_sleep(K_MSEC(10));
	}

	return 0;
}
