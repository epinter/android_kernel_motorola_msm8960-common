/*
 *  * Copyright (C) 2011-2012 Motorola, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA
 */

#ifndef __MMI_EMU_DETECTION_H__
#define __MMI_EMU_DETECTION_H__

struct mmi_emu_det_platform_data {
	int (*enable_5v)(int on);
	int (*core_power)(int on);
	int (*id_protect)(int on);
	int (*alt_mode)(int on);
	void (*gpio_mode)(int mode);
	int (*adc_id)(void);
};

#define GPIO_MODE_GPIO	0
#define GPIO_MODE_GSBI	1

#endif  /* __MMI_EMU_DETECTION_H__ */
