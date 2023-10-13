/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * Copyright (c) 2023 Leon Rinkel
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/can.h>

#include <app_version.h>

#define RX_LED_NODE DT_ALIAS(rx_led)
static const struct gpio_dt_spec rx_led = GPIO_DT_SPEC_GET(RX_LED_NODE, gpios);
#define TX_LED_NODE DT_ALIAS(tx_led)
static const struct gpio_dt_spec tx_led = GPIO_DT_SPEC_GET(TX_LED_NODE, gpios);

#define CANBUS_NODE DT_CHOSEN(zephyr_canbus)
static const struct device* can_dev = DEVICE_DT_GET(CANBUS_NODE);

static void can_rx_callback(
	const struct device* dev, struct can_frame* frame, void* user_data)
{
	int err;

	err = gpio_pin_toggle_dt(&rx_led);
	if (err < 0)
	{
		printk("unable to toggle rx led (err %d)\n", err);
	}

	printk("can %8x %2d", frame->id, frame->dlc);
	for (int i = 0; i < frame->dlc; i++)
	{
		printk(" %02x", frame->data[i]);
	}
	printk("\n");
}

static void can_tx_callback(
	const struct device* dev, int error, void* user_data)
{
	int err;

	err = gpio_pin_toggle_dt(&tx_led);
	if (err < 0)
	{
		printk("unable to toggle tx led (err %d)\n", err);
	}
}

int main(void)
{
	int err;
	struct can_frame frame = {0};
	const struct can_filter filter =
	{
		.flags = CAN_FILTER_DATA | CAN_FILTER_FDF,
		.id = 0x123,
	};

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

	if (!device_is_ready(can_dev))
	{
		printk("can device not ready\n");
		return 0;
	}

	err = can_set_mode(can_dev, CAN_MODE_FD);
	if (err < 0)
	{
		printk("unable to set fd mode (err %d)\n", err);
		return 0;
	}

	err = can_add_rx_filter(can_dev, can_rx_callback, NULL, &filter);
	if (err < 0)
	{
		printk("unable to add filter (err %d)\n", err);
		return 0;
	}

	err = can_start(can_dev);
	if (err < 0)
	{
		printk("unable to start can (err %d)\n", err);
		return 0;
	}

	frame.id = 0x456;
	frame.flags |= CAN_FRAME_FDF;
	frame.flags |= CAN_FRAME_BRS;
	frame.dlc = 3;

	for (unsigned int i = 0;; i++)
	{
		printk("hello world %d\n", i);

		frame.data[0] = (i & 0xFFu);
		frame.data[1] = (i & 0xFFu);
		frame.data[2] = (i & 0xFFu);
		err = can_send(can_dev, &frame,
			K_NO_WAIT, can_tx_callback, NULL);
		if (err < 0)
		{
			printk("failed to enqueue can frame (err %d)\n", err);
		}

		k_sleep(K_MSEC(1000));
	}

	return 0;
}
