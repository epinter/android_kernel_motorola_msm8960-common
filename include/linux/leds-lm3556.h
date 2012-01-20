/*
 * Copyright (C) 2009 Motorola, Inc.
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

#ifndef _LINUX_LED_LM3556_H__
#define _LINUX_LED_LM3556_H__


#define LM3556_LED_DEV "torch-flash"

#define LM3556_NAME "lm3556_led"
#define LM3556_NAME_LEN		(sizeof(LM3556_NAME)-1)

#ifdef __KERNEL__


#define LM3556_ERROR_CHECK	(1 << 0)
#define LM3556_TORCH		(1 << 1)
#define LM3556_FLASH		(1 << 2)


#define LM3556_TORCH_OFF	0
#define LM3556_LOW_TORCH_INTENSITY	63
#define LM3556_MED_LOW_TORCH_INTENSITY	127
#define LM3556_MED_TORCH_INTENSITY	191
#define LM3556_HIGH_TORCH_INTENSITY	255

#define LM3556_TORCH_MODE	0
#define LM3556_STROBE_MODE	1

struct lm3556_platform_data {
	uint32_t flags;
	u8 si_rev_filter_time_def;
	u8 ivfm_reg_def;
	u8 ntc_reg_def;
	u8 ind_ramp_time_reg_def;
	u8 ind_blink_reg_def;
	u8 ind_period_cnt_reg_def;
	u8 torch_ramp_time_reg_def;
	u8 config_reg_def;
	u8 flash_features_reg_def;
	u8 current_cntrl_reg_def;
	u8 torch_brightness_def;
	u8 enable_reg_def;
	u8 flag_reg_def;
	u8 torch_enable_val;
	u8 flash_enable_val;
	u32 hw_enable;
} __packed;

int lm3556_led_write(unsigned long, uint8_t);

#endif	/* __KERNEL__ */

#endif	/* _LINUX_LED_LM3556_H__ */
