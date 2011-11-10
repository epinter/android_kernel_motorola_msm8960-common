/*
 * arch/arm/mach-msm/board-mmi-keypad.c
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


static struct keyreset_platform_data mmi_reset_keys_pdata = {
	.delay = 7000,
	.keys_down = {
		KEY_POWER,
		KEY_VOLUMEUP,
		0
	},
};

struct platform_device mmi_keyreset_device = {
	.name	= KEYRESET_NAME,
	.dev	= {
		.platform_data = &mmi_reset_keys_pdata,
	},
};




static const unsigned int mmi_keymap[] = {
	KEY(0, 0, 0),
	KEY(0, 1, 0),
	KEY(0, 2, KEY_VOLUMEDOWN),
	KEY(0, 3, 0),
	KEY(0, 4, KEY_VOLUMEUP),
};

static struct matrix_keymap_data mmi_keymap_data = {
	.keymap_size    = ARRAY_SIZE(mmi_keymap),
	.keymap         = mmi_keymap,
};

struct pm8xxx_keypad_platform_data mmi_keypad_data = {
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
	.keymap_data            = &mmi_keymap_data,
};

static const unsigned int mmi_qwerty_keymap[] = {

	KEY(0, 0, KEY_9),
	KEY(1, 0, KEY_R),
	KEY(2, 0, KEY_CAMERA),
	KEY(3, 0, 0),
	KEY(4, 0, 0),
	KEY(5, 0, KEY_VOLUMEUP),
	KEY(6, 0, KEY_MINUS),
	KEY(7, 0, KEY_D),

	KEY(0, 1, KEY_7),
	KEY(1, 1, KEY_M),
	KEY(2, 1, KEY_L),
	KEY(3, 1, KEY_K),
	KEY(4, 1, KEY_N),
	KEY(5, 1, KEY_C),
	KEY(6, 1, KEY_Z),
	KEY(7, 1, 0),

	KEY(0, 2, KEY_1),
	KEY(1, 2, KEY_Y),
	KEY(2, 2, KEY_I),
	KEY(3, 2, KEY_SLASH),
	KEY(4, 2, KEY_LEFTALT),
	KEY(5, 2, KEY_DOT),
	KEY(6, 2, KEY_G),
	KEY(7, 2, KEY_E),

	KEY(0, 3, 0),
	KEY(1, 3, 0),
	KEY(2, 3, KEY_3),
	KEY(3, 3, KEY_DOWN),
	KEY(4, 3, KEY_UP),
	KEY(5, 3, KEY_LEFT),
	KEY(6, 3, KEY_RIGHT),
	KEY(7, 3, KEY_REPLY),

	KEY(0, 4, KEY_5),
	KEY(1, 4, KEY_J),
	KEY(2, 4, KEY_B),
	KEY(3, 4, KEY_GRAVE),
	KEY(4, 4, KEY_T),
	KEY(5, 4, KEY_TAB),
	KEY(6, 4, 0),
	KEY(7, 4, KEY_X),

	KEY(0, 5, KEY_8),
	KEY(1, 5, KEY_SPACE),
	KEY(2, 5, KEY_LEFTSHIFT),
	KEY(3, 5, KEY_COMMA),
	KEY(4, 5, KEY_RIGHTALT),
	KEY(5, 5, KEY_6),
	KEY(6, 5, KEY_BACKSPACE),
	KEY(7, 5, 0),

	KEY(0, 6, KEY_2),
	KEY(1, 6, KEY_0),
	KEY(2, 6, KEY_F),
	KEY(3, 6, KEY_CAPSLOCK),
	KEY(4, 6, KEY_ENTER),
	KEY(5, 6, KEY_O),
	KEY(6, 6, KEY_H),
	KEY(7, 6, KEY_Q),

	KEY(0, 7, KEY_4),
	KEY(1, 7, KEY_V),
	KEY(2, 7, KEY_S),
	KEY(3, 7, KEY_P),
	KEY(4, 7, KEY_EQUAL),
	KEY(5, 7, KEY_A),
	KEY(6, 7, KEY_U),
	KEY(7, 7, KEY_W),

};

static struct matrix_keymap_data mmi_qwerty_keymap_data = {
	.keymap_size    = ARRAY_SIZE(mmi_keymap),
	.keymap         = mmi_qwerty_keymap,
};

struct pm8xxx_keypad_platform_data mmi_qwerty_keypad_data = {
	.input_name             = "keypad_8960",
	.input_phys_device      = "keypad_8960/input0",
	.num_rows               = 8,
	.num_cols               = 8,
	.rows_gpio_start	= PM8921_GPIO_PM_TO_SYS(9),
	.cols_gpio_start	= PM8921_GPIO_PM_TO_SYS(1),
	.debounce_ms            = 15,
	.scan_delay_ms          = 32,
	.row_hold_ns            = 91500,
	.wakeup                 = 1,
	.keymap_data            = &mmi_qwerty_keymap_data,
};


#ifdef CONFIG_INPUT_GPIO
static struct gpio_event_direct_entry mmi_switch_map[] = {
	{ 11, SW_LID }
};

static struct gpio_event_input_info mmi_switch_info = {
	.info.func = gpio_event_input_func,
	.flags = 0,
	.type = EV_SW,
	.keymap = mmi_switch_map,
	.keymap_size = ARRAY_SIZE(mmi_switch_map)
};

static struct gpio_event_info *mmi_slide_detect_info[] = {
	&mmi_switch_info.info,
};

static struct gpio_event_platform_data mmi_slide_detect_data = {
	.name = "slide_detect",
	.info = mmi_slide_detect_info,
	.info_count = ARRAY_SIZE(mmi_slide_detect_info)
};

static struct platform_device mmi_slide_detect_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev = {
		.platform_data = &mmi_slide_detect_data,
	},
};
#endif


int __init mmi_keypad_init(int mode)
{
	if (mode & MMI_KEYPAD_RESET)
		platform_device_register(&mmi_keyreset_device);
#ifdef CONFIG_INPUT_GPIO
	if (mode & MMI_KEYPAD_SLIDER)
		platform_device_register(&mmi_slide_detect_device);
#endif
	return 0;
}

