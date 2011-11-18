/*
 * arch/arm/mach-msm/board-mmi.h
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

#ifndef __ARCH_ARM_MACH_MSM_BOARD_MMI_H
#define __ARCH_ARM_MACH_MSM_BOARD_MMI_H

#include <linux/mfd/pm8xxx/pm8921.h>
#include <linux/ct406.h>

#define _HWREV(x)	(x)
#define HWREV_DEF	_HWREV(0xFF00)
#define HWREV_S1	_HWREV(0x1100)
#define HWREV_S2	_HWREV(0x1200)
#define HWREV_S3	_HWREV(0x1300)
#define HWREV_M1	_HWREV(0x2100)
#define HWREV_M2	_HWREV(0x2200)
#define HWREV_M3	_HWREV(0x2300)
#define HWREV_P0	_HWREV(0x8000)
#define HWREV_P1	_HWREV(0x8100)
#define HWREV_P1B2	_HWREV(0x81B2)
#define HWREV_P1C	_HWREV(0x81C0)
#define HWREV_P2	_HWREV(0x8200)
#define HWREV_P3	_HWREV(0x8300)
#define HWREV_P4	_HWREV(0x8400)
#define HWREV_P5	_HWREV(0x8500)
#define HWREV_P6	_HWREV(0x8600)

#define MELFAS_TOUCH_INT_GPIO		46
#define MELFAS_TOUCH_RESET_GPIO		50

#ifdef CONFIG_TOUCHSCREEN_MELFAS100_TS
extern struct melfas_ts_platform_data touch_pdata;
extern int __init melfas_ts_platform_init(void);
#endif

#ifdef CONFIG_INPUT_CT406
#define CT406_IRQ_GPIO 49
extern struct ct406_platform_data mp_ct406_pdata;
#endif

#ifdef CONFIG_BACKLIGHT_LM3532
extern struct lm3532_backlight_platform_data mp_lm3532_pdata;
#endif

#ifdef CONFIG_TOUCHSCREEN_CYTTSP3
extern struct touch_platform_data ts_platform_data_cyttsp3;
extern void mot_setup_touch_cyttsp3(void);
#endif

#ifdef CONFIG_TOUCHSCREEN_ATMXT
extern void mot_setup_touch_atmxt(void);
extern struct touch_platform_data ts_platform_data_atmxt;
#endif

extern struct pm8xxx_keypad_platform_data mmi_keypad_data;
extern struct pm8xxx_keypad_platform_data mmi_qwerty_keypad_data;

extern struct pm8xxx_keypad_platform_data mot_keypad_data;

void __init msm8960_sensors_init(void);
void __init mmi_vibrator_init(void);

#define  MMI_KEYPAD_RESET	0x1
#define  MMI_KEYPAD_SLIDER	0x2

int __init mmi_keypad_init(int mode);
#endif
