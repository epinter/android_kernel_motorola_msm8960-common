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
	.y	= {1775, 1775, 1775},
	.cols	= 3,
};

static struct single_row_lut mmi_eb20_fcc_sf = {
	.x	= {1, 100, 200, 300, 400, 500},
	.y	= {100, 97, 95, 93, 91, 90},
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
			{4321, 4321, 4321},
			{4270, 4270, 4270},
			{4212, 4212, 4212},
			{4155, 4155, 4155},
			{4099, 4099, 4099},
			{4046, 4046, 4046},
			{3994, 3994, 3994},
			{3945, 3945, 3945},
			{3904, 3904, 3904},
			{3868, 3868, 3868},
			{3837, 3837, 3837},
			{3810, 3810, 3810},
			{3786, 3786, 3786},
			{3765, 3765, 3765},
			{3746, 3746, 3746},
			{3729, 3729, 3729},
			{3707, 3707, 3707},
			{3674, 3674, 3674},
			{3645, 3645, 3645},
			{3639, 3639, 3639},
			{3630, 3630, 3630},
			{3618, 3618, 3618},
			{3605, 3605, 3605},
			{3585, 3585, 3585},
			{3555, 3555, 3555},
			{3507, 3507, 3507},
			{3441, 3441, 3441},
			{3348, 3348, 3348},
			{3200, 3200, 3200}
	},
};

static struct pm8921_bms_battery_data  mmi_eb20_metering_data = {
	.fcc			= 1775,
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
	.max_bat_chg_current		= 1775,
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
};

/* EB20 Preliminary Tables and Charging Parameters */
static struct pm8921_bms_battery_data  mmi_eb20_pre_metering_data = {
	.fcc			= 1775,
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
	.max_bat_chg_current		= 1775,
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
};

/* Default Tables and Charging Parameters */
static struct single_row_lut mmi_df_fcc_temp = {
	.x	= {-10, 23, 60},
	.y	= {1775, 1775, 1775},
	.cols	= 3,
};

static struct single_row_lut mmi_df_fcc_sf = {
	.x	= {1, 100, 200, 300, 400, 500},
	.y	= {100, 97, 95, 93, 91, 90},
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
			{4321, 4321, 4321},
			{4270, 4270, 4270},
			{4212, 4212, 4212},
			{4155, 4155, 4155},
			{4099, 4099, 4099},
			{4046, 4046, 4046},
			{3994, 3994, 3994},
			{3945, 3945, 3945},
			{3904, 3904, 3904},
			{3868, 3868, 3868},
			{3837, 3837, 3837},
			{3810, 3810, 3810},
			{3786, 3786, 3786},
			{3765, 3765, 3765},
			{3746, 3746, 3746},
			{3729, 3729, 3729},
			{3707, 3707, 3707},
			{3674, 3674, 3674},
			{3645, 3645, 3645},
			{3639, 3639, 3639},
			{3630, 3630, 3630},
			{3618, 3618, 3618},
			{3605, 3605, 3605},
			{3585, 3585, 3585},
			{3555, 3555, 3555},
			{3507, 3507, 3507},
			{3441, 3441, 3441},
			{3348, 3348, 3348},
			{3200, 3200, 3200}
	},
};

static struct pm8921_bms_battery_data  mmi_df_metering_data = {
	.fcc			= 1775,
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
	.max_bat_chg_current		= 1775,
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
