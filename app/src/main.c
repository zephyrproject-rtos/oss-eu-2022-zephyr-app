/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/led.h>
#include <zephyr/usb/usb_device.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

void main(void)
{
	const struct device *const rgbled =
		DEVICE_DT_GET(DT_NODELABEL(rgbled));

#ifdef CONFIG_USB_DEVICE_STACK
	int ret = usb_enable(NULL);
	if (ret < 0) {
		LOG_ERR("Failed to enable USB");
		return;
	}
#endif

	if (!device_is_ready(rgbled)) {
		LOG_ERR("RGB LED not ready");
		return;
	}

	/* show some basic colors */
	printk("RED\n");
	led_set_color(rgbled, 0U, 3U, (uint8_t []){255U, 0U, 0U});
	k_msleep(1000);
	printk("GREEN\n");
	led_set_color(rgbled, 0U, 3U, (uint8_t []){0U, 255U, 0U});
	k_msleep(1000);
	printk("BLUE\n");
	led_set_color(rgbled, 0U, 3U, (uint8_t []){0U, 0U, 255U});
	k_msleep(1000);
	printk("YELLOW\n");
	led_set_color(rgbled, 0U, 3U, (uint8_t []){255U, 255U, 0U});
	k_msleep(1000);
	printk("CYAN\n");
	led_set_color(rgbled, 0U, 3U, (uint8_t []){0U, 255U, 255U});
	k_msleep(1000);
	printk("MAGENTA\n");
	led_set_color(rgbled, 0U, 3U, (uint8_t []){255U, 0U, 255U});
	k_msleep(1000);

	led_off(rgbled, 0U);
}
