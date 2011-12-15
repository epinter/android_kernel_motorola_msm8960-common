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
#include <linux/gpio.h>
#include <asm/mach-types.h>
#include <linux/regulator/pm8921-regulator.h>
#include <linux/regulator/gpio-regulator.h>
#include <mach/rpm-regulator.h>
#ifdef CONFIG_KEYBOARD_GPIO
#include <linux/gpio_keys.h>
#endif
#include "board-8960.h"
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
	KEY(0, 1, KEY_R),
	KEY(0, 2, KEY_CAMERA),
	KEY(0, 3, 0),
	KEY(0, 4, 0),
	KEY(0, 5, KEY_VOLUMEUP),
	KEY(0, 6, KEY_MINUS),
	KEY(0, 7, KEY_D),

	KEY(1, 0, KEY_7),
	KEY(1, 1, KEY_M),
	KEY(1, 2, KEY_L),
	KEY(1, 3, KEY_K),
	KEY(1, 4, KEY_N),
	KEY(1, 5, KEY_C),
	KEY(1, 6, KEY_Z),
	KEY(1, 7, 0),

	KEY(2, 0, KEY_1),
	KEY(2, 1, KEY_Y),
	KEY(2, 2, KEY_I),
	KEY(2, 3, KEY_SLASH),
	KEY(2, 4, KEY_LEFTALT),
	KEY(2, 5, KEY_DOT),
	KEY(2, 6, KEY_G),
	KEY(2, 7, KEY_E),

	KEY(3, 0, 0),
	KEY(3, 1, 0),
	KEY(3, 2, KEY_3),
	KEY(3, 3, KEY_DOWN),
	KEY(3, 4, KEY_UP),
	KEY(3, 5, KEY_LEFT),
	KEY(3, 6, KEY_RIGHT),
	KEY(3, 7, KEY_REPLY),

	KEY(4, 0, KEY_5),
	KEY(4, 1, KEY_J),
	KEY(4, 2, KEY_B),
	KEY(4, 3, KEY_GRAVE),
	KEY(4, 4, KEY_T),
	KEY(4, 5, KEY_TAB),
	KEY(4, 6, 0),
	KEY(4, 7, KEY_X),

	KEY(5, 0, KEY_8),
	KEY(5, 1, KEY_SPACE),
	KEY(5, 2, KEY_LEFTSHIFT),
	KEY(5, 3, KEY_COMMA),
	KEY(5, 4, KEY_RIGHTALT),
	KEY(5, 5, KEY_6),
	KEY(5, 6, KEY_BACKSPACE),
	KEY(5, 7, 0),

	KEY(6, 0, KEY_2),
	KEY(6, 1, KEY_0),
	KEY(6, 2, KEY_F),
	KEY(6, 3, KEY_CAPSLOCK),
	KEY(6, 4, KEY_ENTER),
	KEY(6, 5, KEY_O),
	KEY(6, 6, KEY_H),
	KEY(6, 7, KEY_Q),

	KEY(7, 0, KEY_4),
	KEY(7, 1, KEY_V),
	KEY(7, 2, KEY_S),
	KEY(7, 3, KEY_P),
	KEY(7, 4, KEY_EQUAL),
	KEY(7, 5, KEY_A),
	KEY(7, 6, KEY_U),
	KEY(7, 7, KEY_W),

};

static struct matrix_keymap_data mmi_qwerty_keymap_data = {
	.keymap_size    = ARRAY_SIZE(mmi_qwerty_keymap),
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


#ifdef CONFIG_KEYBOARD_GPIO

static struct gpio_keys_button mmi_gpio_keys_table[] = {
	{KEY_VOLUMEDOWN, PM8921_GPIO_PM_TO_SYS(33), 1,
			"VOLUME_DOWN", EV_KEY, 1, 20},
	{SW_LID, 11, 0, "SLIDE", EV_SW,  1, 20},
};

static struct gpio_keys_platform_data mmi_gpio_keys_data = {
	.buttons	= mmi_gpio_keys_table,
	.nbuttons	= ARRAY_SIZE(mmi_gpio_keys_table),
	.rep		= false,
};

static struct platform_device mmi_gpiokeys_device = {
	.name		= "gpio-keys",
	.id		= -1,
	.dev = {
		.platform_data = &mmi_gpio_keys_data,
	},
};
#endif
int __init mmi_keypad_init(int mode)
{
	if (mode & MMI_KEYPAD_RESET)
		platform_device_register(&mmi_keyreset_device);
#ifdef CONFIG_KEYBOARD_GPIO
	if (mode & MMI_KEYPAD_SLIDER)
		platform_device_register(&mmi_gpiokeys_device);
#endif
	return 0;
}

