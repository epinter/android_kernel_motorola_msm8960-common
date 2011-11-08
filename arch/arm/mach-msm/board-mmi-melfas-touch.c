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

#include <linux/melfas100_ts.h>
#include "board-mmi.h"

struct melfas_ts_platform_data touch_pdata = {
	.flags = TS_FLIP_X | TS_FLIP_Y,
	};

/*
static struct i2c_board_info i2c_bus3_melfas_ts_info[] __initdata = {
	{
		I2C_BOARD_INFO(MELFAS_TS_NAME, 0x48),
		.irq = MSM_GPIO_TO_INT(MELFAS_TOUCH_INT_GPIO),
		.platform_data = &touch_pdata,
	},
};
*/

int __init melfas_ts_platform_init(void)
{
	pr_info(" MELFAS TS: platform init\n");

	/* melfas reset gpio */
	gpio_request(MELFAS_TOUCH_RESET_GPIO, "touch_reset");
	gpio_direction_output(MELFAS_TOUCH_RESET_GPIO, 1);

	/* melfas interrupt gpio */
	gpio_request(MELFAS_TOUCH_INT_GPIO, "touch_irq");
	gpio_direction_input(MELFAS_TOUCH_INT_GPIO);
	/*
	i2c_register_board_info(3, i2c_bus3_melfas_ts_info,
		ARRAY_SIZE(i2c_bus3_melfas_ts_info));
	*/
	return 0;
}
