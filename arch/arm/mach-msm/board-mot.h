/*
 * arch/arm/mach-msm/board-mot.h
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

#ifndef __ARCH_ARM_MACH_MSM_BOARD_MOT_H
#define __ARCH_ARM_MACH_MSM_BOARD_MOT_H

#include <linux/mfd/pm8xxx/pm8921.h>

extern void __init mot_vibrator_init(void);
extern int __init mot_keypad_init(void);
extern struct pm8xxx_keypad_platform_data mot_keypad_data;
#endif
