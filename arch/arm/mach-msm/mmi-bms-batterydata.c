/* Copyright (c) 2011, Motorola Mobility. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 */
#include <linux/mfd/pm8xxx/pm8921-bms.h>
#include <linux/mfd/pm8xxx/pm8921-charger.h>
#include <linux/power/mmi-battery.h>
#include "board-mmi.h"

/* EB20 Tables and Charging Parameters */
static struct single_row_lut mmi_eb20_fcc_temp = {
	.x	= {-10, 23, 60},
	.y	= {1608, 1767, 1749},
	.cols	= 3,
};

static struct single_row_lut mmi_eb20_fcc_sf = {
	.x	= {1, 100, 200, 300, 400, 500},
	.y	= {100, 97, 95, 93, 92, 90},
	.cols	= 6,
};

static struct pc_sf_lut mmi_eb20_pc_sf = {
	.rows		= 10,
	.cols		= 5,
	.cycles		= {100, 200, 300, 400, 500},
	.percent	= {100, 90, 80, 70, 60, 50, 40, 30, 20, 10},
	.sf		= {
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100}
	},
};

static struct pc_temp_ocv_lut mmi_eb20_pc_temp_ocv = {
	.rows		= 29,
	.cols		= 3,
	.temp		= {-10, 23, 60},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55,
				50, 45, 40, 35, 30, 25, 20, 15, 10,
				9, 8, 7, 6, 5, 4, 3, 2, 1, 0
	},
	.ocv		= {
			{4310, 4310, 4310},
			{4278, 4272, 4260},
			{4231, 4227, 4216},
			{4177, 4172, 4162},
			{4125, 4120, 4111},
			{4076, 4071, 4062},
			{4020, 4023, 4017},
			{3967, 3981, 3975},
			{3923, 3941, 3936},
			{3886, 3892, 3887},
			{3855, 3854, 3851},
			{3831, 3830, 3827},
			{3811, 3810, 3806},
			{3796, 3795, 3788},
			{3784, 3782, 3766},
			{3770, 3768, 3741},
			{3747, 3740, 3716},
			{3715, 3704, 3676},
			{3700, 3687, 3665},
			{3698, 3685, 3663},
			{3695, 3684, 3660},
			{3690, 3680, 3655},
			{3681, 3671, 3640},
			{3659, 3644, 3610},
			{3620, 3601, 3568},
			{3564, 3548, 3514},
			{3492, 3475, 3445},
			{3390, 3376, 3349},
			{3200, 3200, 3200}
	},
};

static struct pm8921_bms_battery_data  mmi_eb20_metering_data = {
	.fcc			= 1767,
	.fcc_temp_lut		= &mmi_eb20_fcc_temp,
	.fcc_sf_lut		= &mmi_eb20_fcc_sf,
	.pc_temp_ocv_lut	= &mmi_eb20_pc_temp_ocv,
	.pc_sf_lut		= &mmi_eb20_pc_sf,
};

static struct pm8921_charger_battery_data mmi_eb20_charging_data = {
	.max_voltage			= 4350,
	.min_voltage			= 3200,
	.resume_voltage_delta		= 100,
	.term_current			= 80,
	.cool_temp			= 0,
	.warm_temp			= 45,
	.max_bat_chg_current		= 1678,
	.cool_bat_chg_current		= 0,
	.warm_bat_chg_current		= 0,
	.cool_bat_voltage		= 3800,
	.warm_bat_voltage		= 3800,
	.step_charge_current		= 1130,
	.step_charge_voltage		= 4200,
};

static struct mmi_battery_cell mmi_eb20_cell_data = {
	.capacity = 0xAF,
	.peak_voltage = 0xB9,
	.dc_impedance = 0x6D,
	.cell_id = 0x0B0A,
};

/* EB20 Preliminary Tables and Charging Parameters */
static struct pm8921_bms_battery_data  mmi_eb20_pre_metering_data = {
	.fcc			= 1767,
	.fcc_temp_lut		= &mmi_eb20_fcc_temp,
	.fcc_sf_lut		= &mmi_eb20_fcc_sf,
	.pc_temp_ocv_lut	= &mmi_eb20_pc_temp_ocv,
	.pc_sf_lut		= &mmi_eb20_pc_sf,
};

static struct pm8921_charger_battery_data mmi_eb20_pre_charging_data = {
	.max_voltage			= 4350,
	.min_voltage			= 3200,
	.resume_voltage_delta		= 100,
	.term_current			= 80,
	.cool_temp			= 0,
	.warm_temp			= 45,
	.max_bat_chg_current		= 1678,
	.cool_bat_chg_current		= 0,
	.warm_bat_chg_current		= 0,
	.cool_bat_voltage		= 3800,
	.warm_bat_voltage		= 3800,
	.step_charge_current		= 1130,
	.step_charge_voltage		= 4200,
};

static struct mmi_battery_cell mmi_eb20_pre_cell_data = {
	.capacity = 0xAE,
	.peak_voltage = 0xB9,
	.dc_impedance = 0x5A,
	.cell_id = 0x0B0A,
};

/* Default Tables and Charging Parameters */
static struct single_row_lut mmi_df_fcc_temp = {
	.x	= {-10, 23, 60},
	.y	= {1608, 1767, 1749},
	.cols	= 3,
};

static struct single_row_lut mmi_df_fcc_sf = {
	.x	= {1, 100, 200, 300, 400, 500},
	.y	= {100, 97, 95, 93, 92, 90},
	.cols	= 6,
};

static struct pc_sf_lut mmi_df_pc_sf = {
	.rows		= 10,
	.cols		= 5,
	.cycles		= {100, 200, 300, 400, 500},
	.percent	= {100, 90, 80, 70, 60, 50, 40, 30, 20, 10},
	.sf		= {
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100},
			{100, 100, 100, 100, 100}
	},
};

static struct pc_temp_ocv_lut mmi_df_pc_temp_ocv = {
	.rows		= 29,
	.cols		= 3,
	.temp		= {-10, 23, 60},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55,
				50, 45, 40, 35, 30, 25, 20, 15, 10,
				9, 8, 7, 6, 5, 4, 3, 2, 1, 0
	},
	.ocv		= {
			{4310, 4310, 4310},
			{4278, 4272, 4260},
			{4231, 4227, 4216},
			{4177, 4172, 4162},
			{4125, 4120, 4111},
			{4076, 4071, 4062},
			{4020, 4023, 4017},
			{3967, 3981, 3975},
			{3923, 3941, 3936},
			{3886, 3892, 3887},
			{3855, 3854, 3851},
			{3831, 3830, 3827},
			{3811, 3810, 3806},
			{3796, 3795, 3788},
			{3784, 3782, 3766},
			{3770, 3768, 3741},
			{3747, 3740, 3716},
			{3715, 3704, 3676},
			{3700, 3687, 3665},
			{3698, 3685, 3663},
			{3695, 3684, 3660},
			{3690, 3680, 3655},
			{3681, 3671, 3640},
			{3659, 3644, 3610},
			{3620, 3601, 3568},
			{3564, 3548, 3514},
			{3492, 3475, 3445},
			{3390, 3376, 3349},
			{3200, 3200, 3200}
	},
};

static struct pm8921_bms_battery_data  mmi_df_metering_data = {
	.fcc			= 1767,
	.fcc_temp_lut		= &mmi_df_fcc_temp,
	.fcc_sf_lut		= &mmi_df_fcc_sf,
	.pc_temp_ocv_lut	= &mmi_df_pc_temp_ocv,
	.pc_sf_lut		= &mmi_df_pc_sf,
};

static struct pm8921_charger_battery_data mmi_df_charging_data = {
	.max_voltage			= 4200,
	.min_voltage			= 3200,
	.resume_voltage_delta		= 100,
	.term_current			= 80,
	.cool_temp			= 0,
	.warm_temp			= 45,
	.max_bat_chg_current		= 1678,
	.cool_bat_chg_current		= 0,
	.warm_bat_chg_current		= 0,
	.cool_bat_voltage		= 3800,
	.warm_bat_voltage		= 3800,
	.step_charge_current		= 0,
	.step_charge_voltage		= 0,
};

static struct mmi_battery_cell mmi_df_cell_data = {
	.capacity = 0xFF,
	.peak_voltage = 0xFF,
	.dc_impedance = 0xFF,
	.cell_id = 0x0000,
};

struct mmi_battery_list mmi_batts = {
	.num_cells = MMI_BATTERY_NUM,
	.cell_list = {
		&mmi_df_cell_data,
		&mmi_eb20_cell_data,
		&mmi_eb20_pre_cell_data,
	},
	.bms_list = {
		 &mmi_df_metering_data,
		 &mmi_eb20_metering_data,
		 &mmi_eb20_pre_metering_data,
	 },
	.chrg_list = {
		 &mmi_df_charging_data,
		 &mmi_eb20_charging_data,
		 &mmi_eb20_pre_charging_data,
	 },
};
