/*
 * arch/arm/mach-msm/board-8960-melfas-touch.c
 *
 * Copyright (C) 2011 Motorola Mobility, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/resource.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <asm/mach-types.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <mach/gpio.h>
#include <mach/gpiomux.h>

#include <linux/melfas100_ts.h>
#include "board-mmi.h"

#define MELFAS_TOUCH_SCL_GPIO 17
#define MELFAS_TOUCH_SDA_GPIO 16

static struct gpiomux_setting i2c_gpio_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting gsbi3_active_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting gsbi3_suspended_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_KEEPER,
};

static struct gpiomux_setting melfas_int_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting melfas_int_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct msm_gpiomux_config gsbi3_gpio_configs[] = {
	{
		.gpio = MELFAS_TOUCH_SDA_GPIO,
		.settings = {
			[GPIOMUX_ACTIVE]    = &i2c_gpio_config,
			[GPIOMUX_SUSPENDED] = &i2c_gpio_config,
		},
	},
	{
		.gpio = MELFAS_TOUCH_SCL_GPIO,
		.settings = {
			[GPIOMUX_ACTIVE]    = &i2c_gpio_config,
			[GPIOMUX_SUSPENDED] = &i2c_gpio_config,
		},
	},
	{
		.gpio = MELFAS_TOUCH_INT_GPIO,
		.settings = {
			[GPIOMUX_ACTIVE]    = &melfas_int_act_cfg,
			[GPIOMUX_SUSPENDED] = &melfas_int_sus_cfg,
		},
	}

};

static struct msm_gpiomux_config gsbi3_i2c_configs[] = {
	{
		.gpio = MELFAS_TOUCH_SDA_GPIO,	/* GSBI3 I2C QUP SDA */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi3_suspended_cfg,
			[GPIOMUX_ACTIVE] = &gsbi3_active_cfg,
		},
	},
	{
		.gpio = MELFAS_TOUCH_SCL_GPIO,	/* GSBI3 I2C QUP SCL */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi3_suspended_cfg,
			[GPIOMUX_ACTIVE] = &gsbi3_active_cfg,
		},
	},
	{
		.gpio = MELFAS_TOUCH_INT_GPIO,
		.settings = {
			[GPIOMUX_ACTIVE]    = &melfas_int_act_cfg,
			[GPIOMUX_SUSPENDED] = &melfas_int_sus_cfg,
		},
	}
};

static int melfas_mux_fw_flash(bool to_gpios)
{
	/* TOUCH_EN is always an output */
	if (to_gpios) {
		gpio_direction_output(MELFAS_TOUCH_INT_GPIO, 0);

		msm_gpiomux_install(gsbi3_gpio_configs,
				ARRAY_SIZE(gsbi3_gpio_configs));
		gpio_direction_output(MELFAS_TOUCH_SCL_GPIO, 0);
		gpio_direction_output(MELFAS_TOUCH_SDA_GPIO, 0);
	} else {
		gpio_direction_output(MELFAS_TOUCH_INT_GPIO, 1);
		gpio_direction_input(MELFAS_TOUCH_INT_GPIO);

		msm_gpiomux_install(gsbi3_i2c_configs,
				ARRAY_SIZE(gsbi3_i2c_configs));
		gpio_direction_output(MELFAS_TOUCH_SCL_GPIO, 1);
		gpio_direction_input(MELFAS_TOUCH_SCL_GPIO);
		gpio_direction_output(MELFAS_TOUCH_SDA_GPIO, 1);
		gpio_direction_input(MELFAS_TOUCH_SDA_GPIO);
	}

	return 0;
}

struct melfas_ts_platform_data touch_pdata = {
	.flags = TS_FLIP_X | TS_FLIP_Y,

	.gpio_resetb	= MELFAS_TOUCH_INT_GPIO,
	.gpio_vdd_en	= MELFAS_TOUCH_RESET_GPIO,
	.gpio_scl	= MELFAS_TOUCH_SCL_GPIO,
	.gpio_sda	= MELFAS_TOUCH_SDA_GPIO,

	.mux_fw_flash	= melfas_mux_fw_flash,
};

int __init melfas_ts_platform_init(void)
{
	pr_info(" MELFAS TS: platform init\n");

	/* melfas reset gpio */
	gpio_request(MELFAS_TOUCH_RESET_GPIO, "touch_reset");
	gpio_direction_output(MELFAS_TOUCH_RESET_GPIO, 1);

	/* melfas interrupt gpio */
	gpio_request(MELFAS_TOUCH_INT_GPIO, "touch_irq");
	gpio_direction_input(MELFAS_TOUCH_INT_GPIO);

	gpio_request(MELFAS_TOUCH_SCL_GPIO, "touch_scl");
	gpio_request(MELFAS_TOUCH_SDA_GPIO, "touch_sda");

	return 0;
}
