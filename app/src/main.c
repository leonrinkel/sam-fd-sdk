/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * Copyright (c) 2023 Leon Rinkel
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#include <app_version.h>

int main(void)
{
	printk("sam-fd example application %s\n", APP_VERSION_STRING);

	for (unsigned int i = 0;; i++)
	{
		printk("hello world %d\n", i);

		k_sleep(K_MSEC(1000));
	}

	return 0;
}
