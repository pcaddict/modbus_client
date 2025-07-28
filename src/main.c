/*
 * Copyright (c) 2020 PHYTEC Messtechnik GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/modbus/modbus.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mbc_sample, LOG_LEVEL_INF);

static int client_iface;

const static struct modbus_iface_param client_param = {
	.mode = MODBUS_MODE_RTU,
	.rx_timeout = 50000,
	.serial = {
		.baud = 9600,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1
	},
};

#define MODBUS_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_modbus_serial)

static int init_modbus_client(void)
{
	const char iface_name[] = {DEVICE_DT_NAME(MODBUS_NODE)};

	client_iface = modbus_iface_get_by_name(iface_name);

	return modbus_init_client(client_iface, client_param);
}

int main(void)
{
	uint16_t result[2];
	// const int32_t sleep = 250;
	static uint8_t node = 1;
	int err;


	if (init_modbus_client()) {
		LOG_ERR("Modbus RTU client initialization failed");
		return 0;
	}

	// err = modbus_read_holding_regs(client_iface, node, 0x0101, result, 1);
	err = modbus_read_input_regs(client_iface, node, 0x01, result, 2);
	if (err != 0) {
		LOG_ERR("FC04 failed with %d", err);
		return 0;
	}

	LOG_HEXDUMP_INF(result, sizeof(result),
			"WR|RD holding register:");
	LOG_INF("Temperature: %d", result[0]);
}
