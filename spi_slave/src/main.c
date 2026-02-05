#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/printk.h>

#define SPI_NODE DT_NODELABEL(spi1)

int main(void)
{
	const struct device *spi_dev;
	int ret;

	uint8_t rx_buf[8] = {0};
	uint8_t tx_buf[8] = {0xAA, 0x55, 0x11, 0x22};

	struct spi_buf rx = {
		.buf = rx_buf,
		.len = sizeof(rx_buf),
	};

	struct spi_buf tx = {
		.buf = tx_buf,
		.len = sizeof(tx_buf),
	};

	struct spi_buf_set rx_set = {
		.buffers = &rx,
		.count = 1,
	};

	struct spi_buf_set tx_set = {
		.buffers = &tx,
		.count = 1,
	};

	struct spi_config cfg = {
		.frequency = 1000000,          /* ignored for slave */
		.operation =
			SPI_OP_MODE_SLAVE |
			SPI_WORD_SET(8) |
			SPI_TRANSFER_MSB |
			0,                          /* SPI MODE 0 */
			.slave = 0,
	};

	spi_dev = DEVICE_DT_GET(SPI_NODE);

	if (!device_is_ready(spi_dev)) {
		printk("SPI device not ready\n");
		return;
	}

	printk("SPI SLAVE ready, waiting for master...\n");

	while (1) {
		memset(rx_buf, 0, sizeof(rx_buf));

		ret = spi_transceive(spi_dev, &cfg, &tx_set, &rx_set);

		if (ret == 0) {
			printk("RX:");
			for (int i = 0; i < sizeof(rx_buf); i++) {
				printk(" %02X", rx_buf[i]);
			}

		printk("\n");

		} else {
			printk("SPI error: %d\n", ret);
		}
	}
}

