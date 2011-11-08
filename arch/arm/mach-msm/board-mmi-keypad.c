/*
 * arch/arm/mach-msm/board-mot-keypad.c
 *
 * Copyright (C) 2011 Motorola, Inc.
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



#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/keyreset.h>
#include <linux/gpio_event.h>
#include <linux/gpio.h>
#include <asm/mach-types.h>
#include <linux/regulator/pm8921-regulator.h>
#include <linux/regulator/gpio-regulator.h>
#include <mach/rpm-regulator.h>

#include "board-msm8960.h"
#include "board-mmi.h"


static struct keyreset_platform_data mot_reset_keys_pdata = {
	.delay = 7000,
	.keys_down = {
		KEY_POWER,
		KEY_VOLUMEUP,
		0
	},
};

struct platform_device mot_keyreset_device = {
	.name	= KEYRESET_NAME,
	.dev	= {
		.platform_data = &mot_reset_keys_pdata,
	},
};




static const unsigned int mot_keymap[] = {
	KEY(0, 0, 0),
	KEY(0, 1, 0),
	KEY(0, 2, KEY_VOLUMEDOWN),
	KEY(0, 3, 0),
	KEY(0, 4, KEY_VOLUMEUP),
};

static struct matrix_keymap_data mot_keymap_data = {
	.keymap_size    = ARRAY_SIZE(mot_keymap),
	.keymap         = mot_keymap,
};

struct pm8xxx_keypad_platform_data mot_keypad_data = {
	.input_name             = "keypad_8960",
	.input_phys_device      = "keypad_8960/input0",
	.num_rows               = 1,
	.num_cols               = 5,
	.rows_gpio_start	= PM8921_GPIO_PM_TO_SYS(9),
	.cols_gpio_start	= PM8921_GPIO_PM_TO_SYS(1),
	.debounce_ms            = 15,
	.scan_delay_ms          = 32,
	.row_hold_ns            = 91500,
	.wakeup                 = 1,
	.keymap_data            = &mot_keymap_data,
};



#ifdef CONFIG_INPUT_GPIO
static struct gpio_event_direct_entry mot_switch_map[] = {
       { 11, SW_LID }
};

static struct gpio_event_input_info mot_switch_info = {
       .info.func = gpio_event_input_func,
       .flags = 0,
       .type = EV_SW,
       .keymap = mot_switch_map,
       .keymap_size = ARRAY_SIZE(mot_switch_map)
};

static struct gpio_event_info *mot_slide_detect_info[] = {
        &mot_switch_info.info,
};

static struct gpio_event_platform_data mot_slide_detect_data = {
       .name = "slide_detect",
       .info = mot_slide_detect_info,
       .info_count = ARRAY_SIZE(mot_slide_detect_info)
};

static struct platform_device mot_slide_detect_device = {
       .name = GPIO_EVENT_DEV_NAME,
       .id = 0,
       .dev = {
              .platform_data = &mot_slide_detect_data,
        },
};
#endif


int __init mot_keypad_init(void)
{
	platform_device_register(&mot_keyreset_device);
#ifdef CONFIG_INPUT_GPIO
	platform_device_register(&mot_slide_detect_device);
#endif
	return 0;
}

