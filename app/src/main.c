/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * Copyright (c) 2023 Leon Rinkel
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys_clock.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/led.h>

#include <app_version.h>

static const struct device* led_dev =
	DEVICE_DT_GET(DT_PARENT(DT_ALIAS(rx_led)));
#define RX_LED_IDX DT_NODE_CHILD_IDX(DT_NODELABEL(rx_led))
#define TX_LED_IDX DT_NODE_CHILD_IDX(DT_NODELABEL(tx_led))

#define LED_MIN_TIME_ON K_MSEC(10)
#define LED_MIN_TIME_OFF K_MSEC(100)

static int rx_led_on = 0;
static k_timepoint_t till_turn_rx_led_off;
static k_timepoint_t till_turn_rx_led_on;

static int tx_led_on = 0;
static k_timepoint_t till_turn_tx_led_off;
static k_timepoint_t till_turn_tx_led_on;

#define CANBUS_NODE DT_CHOSEN(zephyr_canbus)
static const struct device* can_dev = DEVICE_DT_GET(CANBUS_NODE);

static void can_rx_callback(
	const struct device* dev, struct can_frame* frame, void* user_data)
{
	int err;

	if (
		!rx_led_on &&
		sys_timepoint_expired(till_turn_rx_led_on)
	)
	{
		err = led_on(led_dev, RX_LED_IDX);
		if (!err)
		{
			rx_led_on = 1;
			till_turn_rx_led_off =
				sys_timepoint_calc(LED_MIN_TIME_ON);
		}
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

	if (
		!tx_led_on &&
		sys_timepoint_expired(till_turn_tx_led_on)
	)
	{
		err = led_on(led_dev, TX_LED_IDX);
		if (!err)
		{
			tx_led_on = 1;
			till_turn_tx_led_off =
				sys_timepoint_calc(LED_MIN_TIME_ON);
		}
	}
}

int main(void)
{
	int err;
	k_timepoint_t till_transmit =
		sys_timepoint_calc(K_NO_WAIT);
	struct can_frame frame = {0};
	const struct can_filter filter = { .id = 0x123 };

	printk("sam-fd example application %s\n", APP_VERSION_STRING);

	if (!device_is_ready(led_dev))
	{
		printk("led device not ready\n");
		return 0;
	}

	till_turn_rx_led_on = sys_timepoint_calc(K_NO_WAIT);
	till_turn_rx_led_off = sys_timepoint_calc(K_NO_WAIT);
	till_turn_tx_led_on = sys_timepoint_calc(K_NO_WAIT);
	till_turn_tx_led_off = sys_timepoint_calc(K_NO_WAIT);

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
	frame.dlc = 4;

	for (unsigned int i = 0;; i++)
	{
		if (
			rx_led_on &&
			sys_timepoint_expired(till_turn_rx_led_off)
		)
		{
			err = led_off(led_dev, RX_LED_IDX);
			if (!err)
			{
				rx_led_on = 0;
				till_turn_rx_led_on =
					sys_timepoint_calc(LED_MIN_TIME_OFF);
			}
		}

		if (
			tx_led_on &&
			sys_timepoint_expired(till_turn_tx_led_off)
		)
		{
			err = led_off(led_dev, TX_LED_IDX);
			if (!err)
			{
				tx_led_on = 0;
				till_turn_tx_led_on =
					sys_timepoint_calc(LED_MIN_TIME_OFF);
			}
		}

		if (sys_timepoint_expired(till_transmit))
		{
			printk("hello world %d\n", i);

			frame.data[0] = ((i >> 24) & 0xFFu);
			frame.data[1] = ((i >> 16) & 0xFFu);
			frame.data[2] = ((i >>  8) & 0xFFu);
			frame.data[3] = ((i >>  0) & 0xFFu);
			err = can_send(can_dev, &frame,
				K_NO_WAIT, can_tx_callback, NULL);
			if (err < 0)
			{
				printk("failed to enqueue can frame (err %d)\n", err);
			}

			till_transmit = sys_timepoint_calc(K_MSEC(1000));
		}

		k_sleep(K_MSEC(1));
	}

	return 0;
}
