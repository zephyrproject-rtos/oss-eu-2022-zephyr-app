/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT led_rgb_pwm

#include <errno.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(led_rgb_pwm, CONFIG_LED_LOG_LEVEL);

struct led_rgb_pwm_data {
	uint8_t colors[3];
};

struct led_rgb_pwm_config {
	struct pwm_dt_spec red;
	struct pwm_dt_spec green;
	struct pwm_dt_spec blue;
};

static int led_rgb_pwm_on(const struct device *dev, uint32_t led)
{
	struct led_rgb_pwm_data *data = dev->data;

	ARG_UNUSED(led);

	return led_set_color(dev, led, 3U, data->colors);
}

static int led_rgb_pwm_off(const struct device *dev, uint32_t led)
{
	const struct led_rgb_pwm_config *config = dev->config;

	ARG_UNUSED(led);

	pwm_set_dt(&config->red, 0U, 0U);
	pwm_set_dt(&config->green, 0U, 0U);
	pwm_set_dt(&config->blue, 0U, 0U);

	return 0;
}

static int led_rgb_pwm_set_color(const struct device *dev, uint32_t led,
				 uint8_t num_colors, const uint8_t *color)
{
	const struct led_rgb_pwm_config *config = dev->config;
	struct led_rgb_pwm_data *data = dev->data;

	ARG_UNUSED(led);

	if (num_colors != 3U) {
		LOG_ERR("Invalid number of colors");
		return -EINVAL;
	}

	/* cache colors */
	data->colors[0] = color[0];
	data->colors[1] = color[1];
	data->colors[2] = color[2];

	pwm_set_pulse_dt(&config->red,
			 ((uint64_t)config->red.period * color[0]) / UINT8_MAX);
	pwm_set_pulse_dt(&config->green,
			 ((uint64_t)config->green.period * color[1]) / UINT8_MAX);
	pwm_set_pulse_dt(&config->blue,
			 ((uint64_t)config->blue.period * color[2]) / UINT8_MAX);

	return 0;
}

static const struct led_driver_api led_rgb_pwm_api = {
	.on = led_rgb_pwm_on,
	.off = led_rgb_pwm_off,
	.set_color = led_rgb_pwm_set_color,
};

static int led_rgb_pwm_init(const struct device *dev)
{
	const struct led_rgb_pwm_config *config = dev->config;

	if (!device_is_ready(config->red.dev) ||
	    !device_is_ready(config->green.dev) ||
	    !device_is_ready(config->blue.dev)) {
		LOG_ERR("PWM devices not ready");
		return -ENODEV;
	}

	return 0;
}

#define LED_RGB_PWM_DEFINE(i)                                                  \
	static struct led_rgb_pwm_data data##i;                                \
                                                                               \
	static const struct led_rgb_pwm_config config##i = {                   \
		.red = PWM_DT_SPEC_INST_GET_BY_NAME(i, red),                   \
		.green = PWM_DT_SPEC_INST_GET_BY_NAME(i, green),               \
		.blue = PWM_DT_SPEC_INST_GET_BY_NAME(i, blue),                 \
	};                                                                     \
                                                                               \
	DEVICE_DT_INST_DEFINE(i, led_rgb_pwm_init, NULL, &data##i, &config##i, \
			      POST_KERNEL, CONFIG_LED_INIT_PRIORITY,           \
			      &led_rgb_pwm_api);

DT_INST_FOREACH_STATUS_OKAY(LED_RGB_PWM_DEFINE)
