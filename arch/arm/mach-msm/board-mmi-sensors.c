/*
 * linux/arch/arm/mach-msm/board-8960-sensors.c
 *
 * Copyright (C) 2009-2011 Motorola Mobility, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifdef CONFIG_INPUT_CT406
#include <linux/ct406.h>
#endif
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#ifdef CONFIG_BACKLIGHT_LM3532
#include <linux/i2c/lm3532.h>
#endif
#include <linux/platform_device.h>
#include <mach/gpio.h>
#include <mach/gpiomux.h>

#include "board-mmi.h"

#ifdef CONFIG_INPUT_CT406
/*
 * CT406
 */

static struct gpiomux_setting ct406_reset_suspend_config = {
                        .func = GPIOMUX_FUNC_GPIO,
                        .drv = GPIOMUX_DRV_2MA,
                        .pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting ct406_reset_active_config = {
                        .func = GPIOMUX_FUNC_GPIO,
                        .drv = GPIOMUX_DRV_2MA,
                        .pull = GPIOMUX_PULL_NONE,
                        .dir = GPIOMUX_IN,
};

static struct msm_gpiomux_config ct406_irq_gpio_config = {
        .gpio = CT406_IRQ_GPIO,
        .settings = {
                [GPIOMUX_SUSPENDED] = &ct406_reset_suspend_config,
                [GPIOMUX_ACTIVE] = &ct406_reset_active_config,
        },
};

struct ct406_platform_data mp_ct406_pdata = {
	.regulator_name = "",
	.prox_samples_for_noise_floor = 0x05,
	.prox_saturation_threshold = 0x0208,
	.prox_covered_offset = 0x008c,
	.prox_uncovered_offset = 0x0046,
	.prox_recalibrate_offset = 0x0046,
	.als_lens_transmissivity = 20,
};

static int __init ct406_init(void)
{
	int ret = 0;

        msm_gpiomux_install(&ct406_irq_gpio_config, 1);

	ret = gpio_request(CT406_IRQ_GPIO, "ct406 proximity int");
	if (ret) {
		pr_err("ct406 gpio_request failed: %d\n", ret);
		goto fail;
	}

	mp_ct406_pdata.irq = MSM_GPIO_TO_INT(CT406_IRQ_GPIO);

	return 0;

fail:
    gpio_free(CT406_IRQ_GPIO);
    return ret;
}
#else
static int __init ct406_init(void)
{
    return 0;
}
#endif //CONFIG_CT406

#ifdef CONFIG_BACKLIGHT_LM3532
/*
 * LM3532
 */

#define LM3532_RESET_GPIO 12

static struct gpiomux_setting lm3532_reset_suspend_config = {
			.func = GPIOMUX_FUNC_GPIO,
			.drv = GPIOMUX_DRV_2MA,
			.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting lm3532_reset_active_config = {
			.func = GPIOMUX_FUNC_GPIO,
			.drv = GPIOMUX_DRV_2MA,
			.pull = GPIOMUX_PULL_NONE,
			.dir = GPIOMUX_OUT_LOW,
};

static struct msm_gpiomux_config lm3532_reset_gpio_config = {
	.gpio = LM3532_RESET_GPIO,
	.settings = {
		[GPIOMUX_SUSPENDED] = &lm3532_reset_suspend_config,
		[GPIOMUX_ACTIVE] = &lm3532_reset_active_config,
	},
};

static int lm3532_power_up(void)
{
	int ret;

	msm_gpiomux_install(&lm3532_reset_gpio_config, 1);

        ret = gpio_request(LM3532_RESET_GPIO, "lm3532_reset");
        if (ret) {
                pr_err("%s: Request LM3532 reset GPIO failed, ret=%d\n",
			__func__, ret);
                return -ENODEV;
        }

	return 0;
}

static int lm3532_power_down(void)
{
	gpio_free(LM3532_RESET_GPIO);

	return 0;
}

static int lm3532_reset_assert(void)
{
	gpio_set_value(LM3532_RESET_GPIO, 0);

	return 0;
}

static int lm3532_reset_release(void)
{
	gpio_set_value(LM3532_RESET_GPIO, 1);

	return 0;
}

struct lm3532_backlight_platform_data mp_lm3532_pdata = {
	.power_up = lm3532_power_up,
	.power_down = lm3532_power_down,
	.reset_assert = lm3532_reset_assert,
	.reset_release = lm3532_reset_release,

	.led1_controller = LM3532_CNTRL_A,
	.led2_controller = LM3532_CNTRL_B,
	.led3_controller = LM3532_CNTRL_C,

	.ctrl_a_name = "lm3532_bl",
	.ctrl_b_name = "lm3532_led1",
	.ctrl_c_name = "lm3532_led2",

	.susd_ramp = 0xC0,
	.runtime_ramp = 0xC0,

	.als1_res = 0xE0,
	.als2_res = 0xE0,
	.als_dwn_delay = 0x00,
	.als_cfgr = 0x2C,

	.en_ambl_sens = 0,

	.init_delay_ms = 5000,

	.ctrl_a_usage = LM3532_BACKLIGHT_DEVICE,
	.ctrl_a_pwm = 0xC2,
	.ctrl_a_fsc = 0xFF,
	.ctrl_a_l4_daylight = 0xFF,
	.ctrl_a_l3_bright = 0xCC,
	.ctrl_a_l2_office = 0x99,
	.ctrl_a_l1_indoor = 0x66,
	.ctrl_a_l0_dark = 0x33,

	.ctrl_b_usage = LM3532_UNUSED_DEVICE,
	.ctrl_b_pwm = 0x82,
	.ctrl_b_fsc = 0xFF,
	.ctrl_b_l4_daylight = 0xFF,
	.ctrl_b_l3_bright = 0xCC,
	.ctrl_b_l2_office = 0x99,
	.ctrl_b_l1_indoor = 0x66,
	.ctrl_b_l0_dark = 0x33,

	.ctrl_c_usage = LM3532_UNUSED_DEVICE,
	.ctrl_c_pwm = 0x82,
	.ctrl_c_fsc = 0xFF,
	.ctrl_c_l4_daylight = 0xFF,
	.ctrl_c_l3_bright = 0xCC,
	.ctrl_c_l2_office = 0x99,
	.ctrl_c_l1_indoor = 0x66,
	.ctrl_c_l0_dark = 0x33,

	.l4_high = 0xCC,
	.l4_low = 0xCC,
	.l3_high = 0x99,
	.l3_low = 0x99,
	.l2_high = 0x66,
	.l2_low = 0x66,
	.l1_high = 0x33,
	.l1_low = 0x33,

	.boot_brightness = LM3532_MAX_BRIGHTNESS,
};
#endif /* CONFIG_BACKLIGHT_LM3532 */

/*
 * Sensors
 */

void __init msm8960_sensors_init(void)
{
	ct406_init();
}
