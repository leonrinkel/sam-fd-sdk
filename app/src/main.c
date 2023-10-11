/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * Copyright (c) 2023 Leon Rinkel
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include <app_version.h>

#define RX_LED_NODE DT_ALIAS(rx_led)
static const struct gpio_dt_spec rx_led = GPIO_DT_SPEC_GET(RX_LED_NODE, gpios);

#define TX_LED_NODE DT_ALIAS(tx_led)
static const struct gpio_dt_spec tx_led = GPIO_DT_SPEC_GET(TX_LED_NODE, gpios);

int main(void)
{
	int err;

	printk("sam-fd example application %s\n", APP_VERSION_STRING);

	if (!gpio_is_ready_dt(&rx_led))
	{
		printk("rx led node not ready\n");
		return 0;
	}
	if (!gpio_is_ready_dt(&tx_led))
	{
		printk("tx led node not ready\n");
		return 0;
	}

	err = gpio_pin_configure_dt(&rx_led, GPIO_OUTPUT_ACTIVE);
	if (err < 0)
	{
		printk("unable to configure rx led (err %d)\n", err);
		return 0;
	}
	err = gpio_pin_configure_dt(&tx_led, GPIO_OUTPUT_INACTIVE);
	if (err < 0)
	{
		printk("unable to configure tx led (err %d)\n", err);
		return 0;
	}

	for (unsigned int i = 0;; i++)
	{
		printk("hello world %d\n", i);

		err = gpio_pin_toggle_dt(&rx_led);
		if (err < 0)
		{
			printk("unable to toggle rx led (err %d)\n", err);
			return 0;
		}
		err = gpio_pin_toggle_dt(&tx_led);
		if (err < 0)
		{
			printk("unable to toggle tx led (err %d)\n", err);
			return 0;
		}

		k_sleep(K_MSEC(1000));
	}

	return 0;
}
