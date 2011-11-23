/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "msm_sensor.h"
#include "msm.h"
#include "msm_ispif.h"
#define SENSOR_NAME "mt9m114"
#define PLATFORM_DRIVER_NAME "msm_camera_mt9m114"
#define mt9m114_obj mt9m114_##obj

#define CAM2_RESET 76
#define CAM2_ANALOG_EN 82
#define CAM2_DIGITAL_EN_N 89

#define	MT9M114_DEFAULT_MASTER_CLK_RATE	24000000

#define	HOST_COMMAND_0	0x0001
#define	HOST_COMMAND_1  0x0002
#define	HOST_COMMAND_2  0x0004
#define	HOST_COMMAND_3  0x0008
#define	HOST_COMMAND_OK  0x8000
#define	COMMAND_REGISTER	0x0080

#undef CDBG
#define CDBG printk

DEFINE_MUTEX(mt9m114_mut);
static struct msm_sensor_ctrl_t mt9m114_s_ctrl;

static struct msm_camera_i2c_reg_conf mt9m114_start_settings[] = {
	{0xDC00, 0x28},
};

static struct msm_camera_i2c_reg_conf mt9m114_stop_settings[] = {
	{0xDC00, 0x50},
};
static struct msm_camera_i2c_reg_dt_conf mt9m114_recommend_settings[] = {

	//MT9M114_1280x720_720p_MIPI_768_fixed_30fps_EXTCLK_24
	{0x098E, 0,1},
	{0xC97E, 0x01,1},            //cam_sysctl_pll_enable = 1
	{0xC980, 0x0120,2},          //cam_sysctl_pll_divider_m_n = 288
	{0xC982, 0x0700,2},        //cam_sysctl_pll_divider_p = 1792
	{0xC984, 0x8001,2},          //cam_port_output_control = 32769
	{0xC988, 0x0F00,2},          //cam_port_mipi_timing_t_hs_zero = 3840
	{0xC98A, 0x0B07,2},          //cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
	{0xC98C, 0x0D01,2},          //cam_port_mipi_timing_t_clk_post_clk_pre = 3329
	{0xC98E, 0x071D,2},          //cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
	{0xC990, 0x0006,2},          //cam_port_mipi_timing_t_lpx = 6
	{0xC992, 0x0A0C,2},          //cam_port_mipi_timing_init_timing = 2572
	{0xC800, 0x007C,2},          //cam_sensor_cfg_y_addr_start = 124
	{0xC802, 0x0004,2},          //cam_sensor_cfg_x_addr_start = 4
	{0xC804, 0x0353,2},          //cam_sensor_cfg_y_addr_end = 851
	{0xC806, 0x050B,2},          //cam_sensor_cfg_x_addr_end = 1291
	{0xC808, 0x02DC,2},          //cam_sensor_cfg_pixclk = 48000000
	{0xC80A, 0x6C00,2},          //cam_sensor_cfg_pixclk = 48000000
	{0xC80C, 0x0001,2},          //cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x00DB,2},          //cam_sensor_cfg_fine_integ_time_min = 219
	{0xC810, 0x05BD,2},          //cam_sensor_cfg_fine_integ_time_max = 1469
	{0xC812, 0x03E8,2},          //cam_sensor_cfg_frame_length_lines = 1000
	{0xC814, 0x0640,2},          //cam_sensor_cfg_line_length_pck = 1600
	{0xC816, 0x0060,2},          //cam_sensor_cfg_fine_correction = 96
	{0xC818, 0x02D3,2},          //cam_sensor_cfg_cpipe_last_row = 723
	{0xC826, 0x0020,2},          //cam_sensor_cfg_reg_0_data = 32
	{0xC834, 0x0000,2},          //cam_sensor_control_read_mode = 0
	{0xC854, 0x0000,2},          //cam_crop_window_xoffset = 0
	{0xC856, 0x0000,2},          //cam_crop_window_yoffset = 0
	{0xC858, 0x0500,2},          //cam_crop_window_width = 1280
	{0xC85A, 0x02D0,2},          //cam_crop_window_height = 720
	{0xC85C, 0x03,1},            //cam_crop_cropmode = 3
	{0xC868, 0x0500,2},          //cam_output_width = 1280
	{0xC86A, 0x02D0,2},          //cam_output_height = 720
	{0xC878, 0x00,1},            //cam_aet_aemode = 0
	{0xC88C, 0x1E00,2},          //cam_aet_max_frame_rate = 7680
	{0xC88E, 0x1E00,2},          //cam_aet_min_frame_rate = 7680
	{0xC914, 0x0000,2},          //cam_stat_awb_clip_window_xstart = 0
	{0xC916, 0x0000,2},          //cam_stat_awb_clip_window_ystart = 0
	{0xC918, 0x04FF,2},          //cam_stat_awb_clip_window_xend = 1279
	{0xC91A, 0x02CF,2},          //cam_stat_awb_clip_window_yend = 719
	{0xC91C, 0x0000,2},          //cam_stat_ae_initial_window_xstart = 0
	{0xC91E, 0x0000,2},          //cam_stat_ae_initial_window_ystart = 0
	{0xC920, 0x00FF,2},          //cam_stat_ae_initial_window_xend = 255
	{0xC922, 0x008F,2},          //cam_stat_ae_initial_window_yend = 143

	//Sensor optimization
	{0x316A, 0x8270,2},
	{0x316C, 0x8270,2},
	{0x3ED0, 0x2305,2},
	{0x3ED2, 0x77CF,2},
	{0x316E, 0x8202,2},
	{0x3180, 0x87FF,2},
	{0x30D4, 0x6080,2},
	{0xA802, 0x0008,2},            // AE_TRACK_MODE

	// Errata item 1
	{0x3E14, 0xFF39,2},

	// Errata item 3
	// Patch 0203; Feature Recommended; Black level correction fix
	// Patch 02 - Address issue associated black level correction not working correctly in binning mode.
	// Feature Recommended patch for errata item 3
	{0x0982, 0x0001,2},           // ACCESS_CTL_STAT
	{0x098A, 0x5000,2},           // PHYSICAL_ADDRESS_ACCESS
	{0xD000, 0x70CF,2},
	{0xD002, 0xFFFF,2},
	{0xD004, 0xC5D4,2},
	{0xD006, 0x903A,2},
	{0xD008, 0x2144,2},
	{0xD00A, 0x0C00,2},
	{0xD00C, 0x2186,2},
	{0xD00E, 0x0FF3,2},
	{0xD010, 0xB844,2},
	{0xD012, 0xB948,2},
	{0xD014, 0xE082,2},
	{0xD016, 0x20CC,2},
	{0xD018, 0x80E2,2},
	{0xD01A, 0x21CC,2},
	{0xD01C, 0x80A2,2},
	{0xD01E, 0x21CC,2},
	{0xD020, 0x80E2,2},
	{0xD022, 0xF404,2},
	{0xD024, 0xD801,2},
	{0xD026, 0xF003,2},
	{0xD028, 0xD800,2},
	{0xD02A, 0x7EE0,2},
	{0xD02C, 0xC0F1,2},
	{0xD02E, 0x08BA,2},
	{0xD030, 0x0600,2},
	{0xD032, 0xC1A1,2},
	{0xD034, 0x76CF,2},
	{0xD036, 0xFFFF,2},
	{0xD038, 0xC130,2},
	{0xD03A, 0x6E04,2},
	{0xD03C, 0xC040,2},
	{0xD03E, 0x71CF,2},
	{0xD040, 0xFFFF,2},
	{0xD042, 0xC790,2},
	{0xD044, 0x8103,2},
	{0xD046, 0x77CF,2},
	{0xD048, 0xFFFF,2},
	{0xD04A, 0xC7C0,2},
	{0xD04C, 0xE001,2},
	{0xD04E, 0xA103,2},
	{0xD050, 0xD800,2},
	{0xD052, 0x0C6A,2},
	{0xD054, 0x04E0,2},
	{0xD056, 0xB89E,2},
	{0xD058, 0x7508,2},
	{0xD05A, 0x8E1C,2},
	{0xD05C, 0x0809,2},
	{0xD05E, 0x0191,2},
	{0xD060, 0xD801,2},
	{0xD062, 0xAE1D,2},
	{0xD064, 0xE580,2},
	{0xD066, 0x20CA,2},
	{0xD068, 0x0022,2},
	{0xD06A, 0x20CF,2},
	{0xD06C, 0x0522,2},
	{0xD06E, 0x0C5C,2},
	{0xD070, 0x04E2,2},
	{0xD072, 0x21CA,2},
	{0xD074, 0x0062,2},
	{0xD076, 0xE580,2},
	{0xD078, 0xD901,2},
	{0xD07A, 0x79C0,2},
	{0xD07C, 0xD800,2},
	{0xD07E, 0x0BE6,2},
	{0xD080, 0x04E0,2},
	{0xD082, 0xB89E,2},
	{0xD084, 0x70CF,2},
	{0xD086, 0xFFFF,2},
	{0xD088, 0xC8D4,2},
	{0xD08A, 0x9002,2},
	{0xD08C, 0x0857,2},
	{0xD08E, 0x025E,2},
	{0xD090, 0xFFDC,2},
	{0xD092, 0xE080,2},
	{0xD094, 0x25CC,2},
	{0xD096, 0x9022,2},
	{0xD098, 0xF225,2},
	{0xD09A, 0x1700,2},
	{0xD09C, 0x108A,2},
	{0xD09E, 0x73CF,2},
	{0xD0A0, 0xFF00,2},
	{0xD0A2, 0x3174,2},
	{0xD0A4, 0x9307,2},
	{0xD0A6, 0x2A04,2},
	{0xD0A8, 0x103E,2},
	{0xD0AA, 0x9328,2},
	{0xD0AC, 0x2942,2},
	{0xD0AE, 0x7140,2},
	{0xD0B0, 0x2A04,2},
	{0xD0B2, 0x107E,2},
	{0xD0B4, 0x9349,2},
	{0xD0B6, 0x2942,2},
	{0xD0B8, 0x7141,2},
	{0xD0BA, 0x2A04,2},
	{0xD0BC, 0x10BE,2},
	{0xD0BE, 0x934A,2},
	{0xD0C0, 0x2942,2},
	{0xD0C2, 0x714B,2},
	{0xD0C4, 0x2A04,2},
	{0xD0C6, 0x10BE,2},
	{0xD0C8, 0x130C,2},
	{0xD0CA, 0x010A,2},
	{0xD0CC, 0x2942,2},
	{0xD0CE, 0x7142,2},
	{0xD0D0, 0x2250,2},
	{0xD0D2, 0x13CA,2},
	{0xD0D4, 0x1B0C,2},
	{0xD0D6, 0x0284,2},
	{0xD0D8, 0xB307,2},
	{0xD0DA, 0xB328,2},
	{0xD0DC, 0x1B12,2},
	{0xD0DE, 0x02C4,2},
	{0xD0E0, 0xB34A,2},
	{0xD0E2, 0xED88,2},
	{0xD0E4, 0x71CF,2},
	{0xD0E6, 0xFF00,2},
	{0xD0E8, 0x3174,2},
	{0xD0EA, 0x9106,2},
	{0xD0EC, 0xB88F,2},
	{0xD0EE, 0xB106,2},
	{0xD0F0, 0x210A,2},
	{0xD0F2, 0x8340,2},
	{0xD0F4, 0xC000,2},
	{0xD0F6, 0x21CA,2},
	{0xD0F8, 0x0062,2},
	{0xD0FA, 0x20F0,2},
	{0xD0FC, 0x0040,2},
	{0xD0FE, 0x0B02,2},
	{0xD100, 0x0320,2},
	{0xD102, 0xD901,2},
	{0xD104, 0x07F1,2},
	{0xD106, 0x05E0,2},
	{0xD108, 0xC0A1,2},
	{0xD10A, 0x78E0,2},
	{0xD10C, 0xC0F1,2},
	{0xD10E, 0x71CF,2},
	{0xD110, 0xFFFF,2},
	{0xD112, 0xC7C0,2},
	{0xD114, 0xD840,2},
	{0xD116, 0xA900,2},
	{0xD118, 0x71CF,2},
	{0xD11A, 0xFFFF,2},
	{0xD11C, 0xD02C,2},
	{0xD11E, 0xD81E,2},
	{0xD120, 0x0A5A,2},
	{0xD122, 0x04E0,2},
	{0xD124, 0xDA00,2},
	{0xD126, 0xD800,2},
	{0xD128, 0xC0D1,2},
	{0xD12A, 0x7EE0,2},
	{0x098E, 0x0000,2},         // LOGICAL_ADDRESS_ACCESS

	//{0xE000, 0x010C},           // PATCHLDR_LOADER_ADDRESS
	//{0xE002, 0x0203},           // PATCHLDR_PATCH_ID
	//{0xE004, 0x41030202},       // PATCHLDR_FIRMWARE_ID


	//[Load Patch 1003]
	{0x0982, 0x0001,2}, 	// ACCESS_CTL_STAT
	{0x098A, 0x5C10,2}, 	// PHYSICAL_ADDRESS_ACCESS
	{0xDC10, 0xC0F1,2},
	{0xDC12, 0x0CDA,2},
	{0xDC14, 0x0580,2},
	{0xDC16, 0x76CF,2},
	{0xDC18, 0xFF00,2},
	{0xDC1A, 0x2184,2},
	{0xDC1C, 0x9624,2},
	{0xDC1E, 0x218C,2},
	{0xDC20, 0x8FC3,2},
	{0xDC22, 0x75CF,2},
	{0xDC24, 0xFFFF,2},
	{0xDC26, 0xE058,2},
	{0xDC28, 0xF686,2},
	{0xDC2A, 0x1550,2},
	{0xDC2C, 0x1080,2},
	{0xDC2E, 0xE001,2},
	{0xDC30, 0x1D50,2},
	{0xDC32, 0x1002,2},
	{0xDC34, 0x1552,2},
	{0xDC36, 0x1100,2},
	{0xDC38, 0x6038,2},
	{0xDC3A, 0x1D52,2},
	{0xDC3C, 0x1004,2},
	{0xDC3E, 0x1540,2},
	{0xDC40, 0x1080,2},
	{0xDC42, 0x081B,2},
	{0xDC44, 0x00D1,2},
	{0xDC46, 0x8512,2},
	{0xDC48, 0x1000,2},
	{0xDC4A, 0x00C0,2},
	{0xDC4C, 0x7822,2},
	{0xDC4E, 0x2089,2},
	{0xDC50, 0x0FC1,2},
	{0xDC52, 0x2008,2},
	{0xDC54, 0x0F81,2},
	{0xDC56, 0xFFFF,2},
	{0xDC58, 0xFF80,2},
	{0xDC5A, 0x8512,2},
	{0xDC5C, 0x1801,2},
	{0xDC5E, 0x0052,2},
	{0xDC60, 0xA512,2},
	{0xDC62, 0x1544,2},
	{0xDC64, 0x1080,2},
	{0xDC66, 0xB861,2},
	{0xDC68, 0x262F,2},
	{0xDC6A, 0xF007,2},
	{0xDC6C, 0x1D44,2},
	{0xDC6E, 0x1002,2},
	{0xDC70, 0x20CA,2},
	{0xDC72, 0x0021,2},
	{0xDC74, 0x20CF,2},
	{0xDC76, 0x04E1,2},
	{0xDC78, 0x0850,2},
	{0xDC7A, 0x04A1,2},
	{0xDC7C, 0x21CA,2},
	{0xDC7E, 0x0021,2},
	{0xDC80, 0x1542,2},
	{0xDC82, 0x1140,2},
	{0xDC84, 0x8D2C,2},
	{0xDC86, 0x6038,2},
	{0xDC88, 0x1D42,2},
	{0xDC8A, 0x1004,2},
	{0xDC8C, 0x1542,2},
	{0xDC8E, 0x1140,2},
	{0xDC90, 0xB601,2},
	{0xDC92, 0x046D,2},
	{0xDC94, 0x0580,2},
	{0xDC96, 0x78E0,2},
	{0xDC98, 0xD800,2},
	{0xDC9A, 0xB893,2},
	{0xDC9C, 0x002D,2},
	{0xDC9E, 0x04A0,2},
	{0xDCA0, 0xD900,2},
	{0xDCA2, 0x78E0,2},
	{0xDCA4, 0x72CF,2},
	{0xDCA6, 0xFFFF,2},
	{0xDCA8, 0xE058,2},
	{0xDCAA, 0x2240,2},
	{0xDCAC, 0x0340,2},
	{0xDCAE, 0xA212,2},
	{0xDCB0, 0x208A,2},
	{0xDCB2, 0x0FFF,2},
	{0xDCB4, 0x1A42,2},
	{0xDCB6, 0x0004,2},
	{0xDCB8, 0xD830,2},
	{0xDCBA, 0x1A44,2},
	{0xDCBC, 0x0002,2},
	{0xDCBE, 0xD800,2},
	{0xDCC0, 0x1A50,2},
	{0xDCC2, 0x0002,2},
	{0xDCC4, 0x1A52,2},
	{0xDCC6, 0x0004,2},
	{0xDCC8, 0x1242,2},
	{0xDCCA, 0x0140,2},
	{0xDCCC, 0x8A2C,2},
	{0xDCCE, 0x6038,2},
	{0xDCD0, 0x1A42,2},
	{0xDCD2, 0x0004,2},
	{0xDCD4, 0x1242,2},
	{0xDCD6, 0x0141,2},
	{0xDCD8, 0x70CF,2},
	{0xDCDA, 0xFF00,2},
	{0xDCDC, 0x2184,2},
	{0xDCDE, 0xB021,2},
	{0xDCE0, 0xD800,2},
	{0xDCE2, 0xB893,2},
	{0xDCE4, 0x07E5,2},
	{0xDCE6, 0x0460,2},
	{0xDCE8, 0xD901,2},
	{0xDCEA, 0x78E0,2},
	{0xDCEC, 0xC0F1,2},
	{0xDCEE, 0x0BFA,2},
	{0xDCF0, 0x05A0,2},
	{0xDCF2, 0x216F,2},
	{0xDCF4, 0x0043,2},
	{0xDCF6, 0xC1A4,2},
	{0xDCF8, 0x220A,2},
	{0xDCFA, 0x1F80,2},
	{0xDCFC, 0xFFFF,2},
	{0xDCFE, 0xE058,2},
	{0xDD00, 0x2240,2},
	{0xDD02, 0x134F,2},
	{0xDD04, 0x1A48,2},
	{0xDD06, 0x13C0,2},
	{0xDD08, 0x1248,2},
	{0xDD0A, 0x1002,2},
	{0xDD0C, 0x70CF,2},
	{0xDD0E, 0x7FFF,2},
	{0xDD10, 0xFFFF,2},
	{0xDD12, 0xE230,2},
	{0xDD14, 0xC240,2},
	{0xDD16, 0xDA00,2},
	{0xDD18, 0xF00C,2},
	{0xDD1A, 0x1248,2},
	{0xDD1C, 0x1003,2},
	{0xDD1E, 0x1301,2},
	{0xDD20, 0x04CB,2},
	{0xDD22, 0x7261,2},
	{0xDD24, 0x2108,2},
	{0xDD26, 0x0081,2},
	{0xDD28, 0x2009,2},
	{0xDD2A, 0x0080,2},
	{0xDD2C, 0x1A48,2},
	{0xDD2E, 0x10C0,2},
	{0xDD30, 0x1248,2},
	{0xDD32, 0x100B,2},
	{0xDD34, 0xC300,2},
	{0xDD36, 0x0BE7,2},
	{0xDD38, 0x90C4,2},
	{0xDD3A, 0x2102,2},
	{0xDD3C, 0x0003,2},
	{0xDD3E, 0x238C,2},
	{0xDD40, 0x8FC3,2},
	{0xDD42, 0xF6C7,2},
	{0xDD44, 0xDAFF,2},
	{0xDD46, 0x1A05,2},
	{0xDD48, 0x1082,2},
	{0xDD4A, 0xC241,2},
	{0xDD4C, 0xF005,2},
	{0xDD4E, 0x7A6F,2},
	{0xDD50, 0xC241,2},
	{0xDD52, 0x1A05,2},
	{0xDD54, 0x10C2,2},
	{0xDD56, 0x2000,2},
	{0xDD58, 0x8040,2},
	{0xDD5A, 0xDA00,2},
	{0xDD5C, 0x20C0,2},
	{0xDD5E, 0x0064,2},
	{0xDD60, 0x781C,2},
	{0xDD62, 0xC042,2},
	{0xDD64, 0x1C0E,2},
	{0xDD66, 0x3082,2},
	{0xDD68, 0x1A48,2},
	{0xDD6A, 0x13C0,2},
	{0xDD6C, 0x7548,2},
	{0xDD6E, 0x7348,2},
	{0xDD70, 0x7148,2},
	{0xDD72, 0x7648,2},
	{0xDD74, 0xF002,2},
	{0xDD76, 0x7608,2},
	{0xDD78, 0x1248,2},
	{0xDD7A, 0x1000,2},
	{0xDD7C, 0x1400,2},
	{0xDD7E, 0x300B,2},
	{0xDD80, 0x084D,2},
	{0xDD82, 0x02C5,2},
	{0xDD84, 0x1248,2},
	{0xDD86, 0x1000,2},
	{0xDD88, 0xE101,2},
	{0xDD8A, 0x1001,2},
	{0xDD8C, 0x04CB,2},
	{0xDD8E, 0x1A48,2},
	{0xDD90, 0x1000,2},
	{0xDD92, 0x7361,2},
	{0xDD94, 0x1408,2},
	{0xDD96, 0x300B,2},
	{0xDD98, 0x2302,2},
	{0xDD9A, 0x02C0,2},
	{0xDD9C, 0x780D,2},
	{0xDD9E, 0x2607,2},
	{0xDDA0, 0x903E,2},
	{0xDDA2, 0x07D6,2},
	{0xDDA4, 0xFFE3,2},
	{0xDDA6, 0x792F,2},
	{0xDDA8, 0x09CF,2},
	{0xDDAA, 0x8152,2},
	{0xDDAC, 0x1248,2},
	{0xDDAE, 0x100E,2},
	{0xDDB0, 0x2400,2},
	{0xDDB2, 0x334B,2},
	{0xDDB4, 0xE501,2},
	{0xDDB6, 0x7EE2,2},
	{0xDDB8, 0x0DBF,2},
	{0xDDBA, 0x90F2,2},
	{0xDDBC, 0x1B0C,2},
	{0xDDBE, 0x1382,2},
	{0xDDC0, 0xC123,2},
	{0xDDC2, 0x140E,2},
	{0xDDC4, 0x3080,2},
	{0xDDC6, 0x7822,2},
	{0xDDC8, 0x1A07,2},
	{0xDDCA, 0x1002,2},
	{0xDDCC, 0x124C,2},
	{0xDDCE, 0x1000,2},
	{0xDDD0, 0x120B,2},
	{0xDDD2, 0x1081,2},
	{0xDDD4, 0x1207,2},
	{0xDDD6, 0x1083,2},
	{0xDDD8, 0x2142,2},
	{0xDDDA, 0x004B,2},
	{0xDDDC, 0x781B,2},
	{0xDDDE, 0x0B21,2},
	{0xDDE0, 0x02E2,2},
	{0xDDE2, 0x1A4C,2},
	{0xDDE4, 0x1000,2},
	{0xDDE6, 0xE101,2},
	{0xDDE8, 0x0915,2},
	{0xDDEA, 0x00C2,2},
	{0xDDEC, 0xC101,2},
	{0xDDEE, 0x1204,2},
	{0xDDF0, 0x1083,2},
	{0xDDF2, 0x090D,2},
	{0xDDF4, 0x00C2,2},
	{0xDDF6, 0xE001,2},
	{0xDDF8, 0x1A4C,2},
	{0xDDFA, 0x1000,2},
	{0xDDFC, 0x1A06,2},
	{0xDDFE, 0x1002,2},
	{0xDE00, 0x234A,2},
	{0xDE02, 0x1000,2},
	{0xDE04, 0x7169,2},
	{0xDE06, 0xF008,2},
	{0xDE08, 0x2053,2},
	{0xDE0A, 0x0003,2},
	{0xDE0C, 0x6179,2},
	{0xDE0E, 0x781C,2},
	{0xDE10, 0x2340,2},
	{0xDE12, 0x104B,2},
	{0xDE14, 0x1203,2},
	{0xDE16, 0x1083,2},
	{0xDE18, 0x0BF1,2},
	{0xDE1A, 0x90C2,2},
	{0xDE1C, 0x1202,2},
	{0xDE1E, 0x1080,2},
	{0xDE20, 0x091D,2},
	{0xDE22, 0x0004,2},
	{0xDE24, 0x70CF,2},
	{0xDE26, 0xFFFF,2},
	{0xDE28, 0xC644,2},
	{0xDE2A, 0x881B,2},
	{0xDE2C, 0xE0B2,2},
	{0xDE2E, 0xD83C,2},
	{0xDE30, 0x20CA,2},
	{0xDE32, 0x0CA2,2},
	{0xDE34, 0x1A01,2},
	{0xDE36, 0x1002,2},
	{0xDE38, 0x1A4C,2},
	{0xDE3A, 0x1080,2},
	{0xDE3C, 0x02B9,2},
	{0xDE3E, 0x05A0,2},
	{0xDE40, 0xC0A4,2},
	{0xDE42, 0x78E0,2},
	{0xDE44, 0xC0F1,2},
	{0xDE46, 0xFF95,2},
	{0xDE48, 0xD800,2},
	{0xDE4A, 0x71CF,2},
	{0xDE4C, 0xFF00,2},
	{0xDE4E, 0x1FE0,2},
	{0xDE50, 0x19D0,2},
	{0xDE52, 0x001C,2},
	{0xDE54, 0x19D1,2},
	{0xDE56, 0x001C,2},
	{0xDE58, 0x70CF,2},
	{0xDE5A, 0xFFFF,2},
	{0xDE5C, 0xE058,2},
	{0xDE5E, 0x901F,2},
	{0xDE60, 0xB861,2},
	{0xDE62, 0x19D2,2},
	{0xDE64, 0x001C,2},
	{0xDE66, 0xC0D1,2},
	{0xDE68, 0x7EE0,2},
	{0xDE6A, 0x78E0,2},
	{0xDE6C, 0xC0F1,2},
	{0xDE6E, 0x0A7A,2},
	{0xDE70, 0x0580,2},
	{0xDE72, 0x70CF,2},
	{0xDE74, 0xFFFF,2},
	{0xDE76, 0xC5D4,2},
	{0xDE78, 0x9041,2},
	{0xDE7A, 0x9023,2},
	{0xDE7C, 0x75CF,2},
	{0xDE7E, 0xFFFF,2},
	{0xDE80, 0xE058,2},
	{0xDE82, 0x7942,2},
	{0xDE84, 0xB967,2},
	{0xDE86, 0x7F30,2},
	{0xDE88, 0xB53F,2},
	{0xDE8A, 0x71CF,2},
	{0xDE8C, 0xFFFF,2},
	{0xDE8E, 0xC84C,2},
	{0xDE90, 0x91D3,2},
	{0xDE92, 0x108B,2},
	{0xDE94, 0x0081,2},
	{0xDE96, 0x2615,2},
	{0xDE98, 0x1380,2},
	{0xDE9A, 0x090F,2},
	{0xDE9C, 0x0C91,2},
	{0xDE9E, 0x0A8E,2},
	{0xDEA0, 0x05A0,2},
	{0xDEA2, 0xD906,2},
	{0xDEA4, 0x7E10,2},
	{0xDEA6, 0x2615,2},
	{0xDEA8, 0x1380,2},
	{0xDEAA, 0x0A82,2},
	{0xDEAC, 0x05A0,2},
	{0xDEAE, 0xD960,2},
	{0xDEB0, 0x790F,2},
	{0xDEB2, 0x090D,2},
	{0xDEB4, 0x0133,2},
	{0xDEB6, 0xAD0C,2},
	{0xDEB8, 0xD904,2},
	{0xDEBA, 0xAD2C,2},
	{0xDEBC, 0x79EC,2},
	{0xDEBE, 0x2941,2},
	{0xDEC0, 0x7402,2},
	{0xDEC2, 0x71CF,2},
	{0xDEC4, 0xFF00,2},
	{0xDEC6, 0x2184,2},
	{0xDEC8, 0xB142,2},
	{0xDECA, 0x1906,2},
	{0xDECC, 0x0E44,2},
	{0xDECE, 0xFFDE,2},
	{0xDED0, 0x70C9,2},
	{0xDED2, 0x0A5A,2},
	{0xDED4, 0x05A0,2},
	{0xDED6, 0x8D2C,2},
	{0xDED8, 0xAD0B,2},
	{0xDEDA, 0xD800,2},
	{0xDEDC, 0xAD01,2},
	{0xDEDE, 0x0219,2},
	{0xDEE0, 0x05A0,2},
	{0xDEE2, 0xA513,2},
	{0xDEE4, 0xC0F1,2},
	{0xDEE6, 0x71CF,2},
	{0xDEE8, 0xFFFF,2},
	{0xDEEA, 0xC644,2},
	{0xDEEC, 0xA91B,2},
	{0xDEEE, 0xD902,2},
	{0xDEF0, 0x70CF,2},
	{0xDEF2, 0xFFFF,2},
	{0xDEF4, 0xC84C,2},
	{0xDEF6, 0x093E,2},
	{0xDEF8, 0x03A0,2},
	{0xDEFA, 0xA826,2},
	{0xDEFC, 0xFFDC,2},
	{0xDEFE, 0xF1B5,2},
	{0xDF00, 0xC0F1,2},
	{0xDF02, 0x09EA,2},
	{0xDF04, 0x0580,2},
	{0xDF06, 0x75CF,2},
	{0xDF08, 0xFFFF,2},
	{0xDF0A, 0xE058,2},
	{0xDF0C, 0x1540,2},
	{0xDF0E, 0x1080,2},
	{0xDF10, 0x08A7,2},
	{0xDF12, 0x0010,2},
	{0xDF14, 0x8D00,2},
	{0xDF16, 0x0813,2},
	{0xDF18, 0x009E,2},
	{0xDF1A, 0x1540,2},
	{0xDF1C, 0x1081,2},
	{0xDF1E, 0xE181,2},
	{0xDF20, 0x20CA,2},
	{0xDF22, 0x00A1,2},
	{0xDF24, 0xF24B,2},
	{0xDF26, 0x1540,2},
	{0xDF28, 0x1081,2},
	{0xDF2A, 0x090F,2},
	{0xDF2C, 0x0050,2},
	{0xDF2E, 0x1540,2},
	{0xDF30, 0x1081,2},
	{0xDF32, 0x0927,2},
	{0xDF34, 0x0091,2},
	{0xDF36, 0x1550,2},
	{0xDF38, 0x1081,2},
	{0xDF3A, 0xDE00,2},
	{0xDF3C, 0xAD2A,2},
	{0xDF3E, 0x1D50,2},
	{0xDF40, 0x1382,2},
	{0xDF42, 0x1552,2},
	{0xDF44, 0x1101,2},
	{0xDF46, 0x1D52,2},
	{0xDF48, 0x1384,2},
	{0xDF4A, 0xB524,2},
	{0xDF4C, 0x082D,2},
	{0xDF4E, 0x015F,2},
	{0xDF50, 0xFF55,2},
	{0xDF52, 0xD803,2},
	{0xDF54, 0xF033,2},
	{0xDF56, 0x1540,2},
	{0xDF58, 0x1081,2},
	{0xDF5A, 0x0967,2},
	{0xDF5C, 0x00D1,2},
	{0xDF5E, 0x1550,2},
	{0xDF60, 0x1081,2},
	{0xDF62, 0xDE00,2},
	{0xDF64, 0xAD2A,2},
	{0xDF66, 0x1D50,2},
	{0xDF68, 0x1382,2},
	{0xDF6A, 0x1552,2},
	{0xDF6C, 0x1101,2},
	{0xDF6E, 0x1D52,2},
	{0xDF70, 0x1384,2},
	{0xDF72, 0xB524,2},
	{0xDF74, 0x0811,2},
	{0xDF76, 0x019E,2},
	{0xDF78, 0xB8A0,2},
	{0xDF7A, 0xAD00,2},
	{0xDF7C, 0xFF47,2},
	{0xDF7E, 0x1D40,2},
	{0xDF80, 0x1382,2},
	{0xDF82, 0xF01F,2},
	{0xDF84, 0xFF5A,2},
	{0xDF86, 0x8D01,2},
	{0xDF88, 0x8D40,2},
	{0xDF8A, 0xE812,2},
	{0xDF8C, 0x71CF,2},
	{0xDF8E, 0xFFFF,2},
	{0xDF90, 0xC644,2},
	{0xDF92, 0x893B,2},
	{0xDF94, 0x7030,2},
	{0xDF96, 0x22D1,2},
	{0xDF98, 0x8062,2},
	{0xDF9A, 0xF20A,2},
	{0xDF9C, 0x0A0F,2},
	{0xDF9E, 0x009E,2},
	{0xDFA0, 0x71CF,2},
	{0xDFA2, 0xFFFF,2},
	{0xDFA4, 0xC84C,2},
	{0xDFA6, 0x893B,2},
	{0xDFA8, 0xE902,2},
	{0xDFAA, 0xFFCF,2},
	{0xDFAC, 0x8D00,2},
	{0xDFAE, 0xB8E7,2},
	{0xDFB0, 0x26CA,2},
	{0xDFB2, 0x1022,2},
	{0xDFB4, 0xF5E2,2},
	{0xDFB6, 0xFF3C,2},
	{0xDFB8, 0xD801,2},
	{0xDFBA, 0x1D40,2},
	{0xDFBC, 0x1002,2},
	{0xDFBE, 0x0141,2},
	{0xDFC0, 0x0580,2},
	{0xDFC2, 0x78E0,2},
	{0xDFC4, 0xC0F1,2},
	{0xDFC6, 0xC5E1,2},
	{0xDFC8, 0xFF34,2},
	{0xDFCA, 0xDD00,2},
	{0xDFCC, 0x70CF,2},
	{0xDFCE, 0xFFFF,2},
	{0xDFD0, 0xE090,2},
	{0xDFD2, 0xA8A8,2},
	{0xDFD4, 0xD800,2},
	{0xDFD6, 0xB893,2},
	{0xDFD8, 0x0C8A,2},
	{0xDFDA, 0x0460,2},
	{0xDFDC, 0xD901,2},
	{0xDFDE, 0x71CF,2},
	{0xDFE0, 0xFFFF,2},
	{0xDFE2, 0xDC10,2},
	{0xDFE4, 0xD813,2},
	{0xDFE6, 0x0B96,2},
	{0xDFE8, 0x0460,2},
	{0xDFEA, 0x72A9,2},
	{0xDFEC, 0x0119,2},
	{0xDFEE, 0x0580,2},
	{0xDFF0, 0xC0F1,2},
	{0xDFF2, 0x71CF,2},
	{0xDFF4, 0x0000,2},
	{0xDFF6, 0x5BAE,2},
	{0xDFF8, 0x7940,2},
	{0xDFFA, 0xFF9D,2},
	{0xDFFC, 0xF135,2},
	{0xDFFE, 0x78E0,2},
	{0xE000, 0xC0F1,2},
	{0xE002, 0x70CF,2},
	{0xE004, 0x0000,2},
	{0xE006, 0x5CBA,2},
	{0xE008, 0x7840,2},
	{0xE00A, 0x70CF,2},
	{0xE00C, 0xFFFF,2},
	{0xE00E, 0xE058,2},
	{0xE010, 0x8800,2},
	{0xE012, 0x0815,2},
	{0xE014, 0x001E,2},
	{0xE016, 0x70CF,2},
	{0xE018, 0xFFFF,2},
	{0xE01A, 0xC84C,2},
	{0xE01C, 0x881A,2},
	{0xE01E, 0xE080,2},
	{0xE020, 0x0EE0,2},
	{0xE022, 0xFFC1,2},
	{0xE024, 0xF121,2},
	{0xE026, 0x78E0,2},
	{0xE028, 0xC0F1,2},
	{0xE02A, 0xD900,2},
	{0xE02C, 0xF009,2},
	{0xE02E, 0x70CF,2},
	{0xE030, 0xFFFF,2},
	{0xE032, 0xE0AC,2},
	{0xE034, 0x7835,2},
	{0xE036, 0x8041,2},
	{0xE038, 0x8000,2},
	{0xE03A, 0xE102,2},
	{0xE03C, 0xA040,2},
	{0xE03E, 0x09F3,2},
	{0xE040, 0x8114,2},
	{0xE042, 0x71CF,2},
	{0xE044, 0xFFFF,2},
	{0xE046, 0xE058,2},
	{0xE048, 0x70CF,2},
	{0xE04A, 0xFFFF,2},
	{0xE04C, 0xC594,2},
	{0xE04E, 0xB030,2},
	{0xE050, 0xFFDD,2},
	{0xE052, 0xD800,2},
	{0xE054, 0xF109,2},
	{0xE056, 0x0000,2},
	{0xE058, 0x0300,2},
	{0xE05A, 0x0204,2},
	{0xE05C, 0x0700,2},
	{0xE05E, 0x0000,2},
	{0xE060, 0x0000,2},
	{0xE062, 0x0000,2},
	{0xE064, 0x0000,2},
	{0xE066, 0x0000,2},
	{0xE068, 0x0000,2},
	{0xE06A, 0x0000,2},
	{0xE06C, 0x0000,2},
	{0xE06E, 0x0000,2},
	{0xE070, 0x0000,2},
	{0xE072, 0x0000,2},
	{0xE074, 0x0000,2},
	{0xE076, 0x0000,2},
	{0xE078, 0x0000,2},
	{0xE07A, 0x0000,2},
	{0xE07C, 0x0000,2},
	{0xE07E, 0x0000,2},
	{0xE080, 0x0000,2},
	{0xE082, 0x0000,2},
	{0xE084, 0x0000,2},
	{0xE086, 0x0000,2},
	{0xE088, 0x0000,2},
	{0xE08A, 0x0000,2},
	{0xE08C, 0x0000,2},
	{0xE08E, 0x0000,2},
	{0xE090, 0x0000,2},
	{0xE092, 0x0000,2},
	{0xE094, 0x0000,2},
	{0xE096, 0x0000,2},
	{0xE098, 0x0000,2},
	{0xE09A, 0x0000,2},
	{0xE09C, 0x0000,2},
	{0xE09E, 0x0000,2},
	{0xE0A0, 0x0000,2},
	{0xE0A2, 0x0000,2},
	{0xE0A4, 0x0000,2},
	{0xE0A6, 0x0000,2},
	{0xE0A8, 0x0000,2},
	{0xE0AA, 0x0000,2},
	{0xE0AC, 0xFFFF,2},
	{0xE0AE, 0xCB68,2},
	{0xE0B0, 0xFFFF,2},
	{0xE0B2, 0xDFF0,2},
	{0xE0B4, 0xFFFF,2},
	{0xE0B6, 0xCB6C,2},
	{0xE0B8, 0xFFFF,2},
	{0xE0BA, 0xE000,2},
	{0x098E, 0x0000,2},	// LOGICAL_ADDRESS_ACCESS

	//REG= 0xE000, 0x1028 	// PATCHLDR_LOADER_ADDRESS
	//REG= 0xE002, 0x1003 	// PATCHLDR_PATCH_ID
	//REG= 0xE004, 0x41030202 	// PATCHLDR_FIRMWARE_ID


	//MIPI setting for SOC1040
	{0x3C5A, 0x0009,2},         // RESERVED_TX_SS_3C5A this line is only needed for Aptina demo camera
	{0x3C44, 0x0080,2},         // MIPI_CUSTOM_SHORT_PKT

	//[Tuning_settings]

	//[CCM]
	{0xC892, 0x0267,2},           // CAM_AWB_CCM_L_0
	{0xC894, 0xFF1A,2},           // CAM_AWB_CCM_L_1
	{0xC896, 0xFFB3,2},           // CAM_AWB_CCM_L_2
	{0xC898, 0xFF80,2},           // CAM_AWB_CCM_L_3
	{0xC89A, 0x0166,2},           // CAM_AWB_CCM_L_4
	{0xC89C, 0x0003,2},           // CAM_AWB_CCM_L_5
	{0xC89E, 0xFF9A,2},           // CAM_AWB_CCM_L_6
	{0xC8A0, 0xFEB4,2},           // CAM_AWB_CCM_L_7
	{0xC8A2, 0x024D,2},           // CAM_AWB_CCM_L_8
	{0xC8A4, 0x01BF,2},           // CAM_AWB_CCM_M_0
	{0xC8A6, 0xFF01,2},           // CAM_AWB_CCM_M_1
	{0xC8A8, 0xFFF3,2},           // CAM_AWB_CCM_M_2
	{0xC8AA, 0xFF75,2},           // CAM_AWB_CCM_M_3
	{0xC8AC, 0x0198,2},           // CAM_AWB_CCM_M_4
	{0xC8AE, 0xFFFD,2},           // CAM_AWB_CCM_M_5
	{0xC8B0, 0xFF9A,2},           // CAM_AWB_CCM_M_6
	{0xC8B2, 0xFEE7,2},           // CAM_AWB_CCM_M_7
	{0xC8B4, 0x02A8,2},           // CAM_AWB_CCM_M_8
	{0xC8B6, 0x01D9,2},           // CAM_AWB_CCM_R_0
	{0xC8B8, 0xFF26,2},           // CAM_AWB_CCM_R_1
	{0xC8BA, 0xFFF3,2},           // CAM_AWB_CCM_R_2
	{0xC8BC, 0xFFB3,2},           // CAM_AWB_CCM_R_3
	{0xC8BE, 0x0132,2},           // CAM_AWB_CCM_R_4
	{0xC8C0, 0xFFE8,2},           // CAM_AWB_CCM_R_5
	{0xC8C2, 0xFFDA,2},           // CAM_AWB_CCM_R_6
	{0xC8C4, 0xFECD,2},           // CAM_AWB_CCM_R_7
	{0xC8C6, 0x02C2,2},           // CAM_AWB_CCM_R_8
	{0xC8C8, 0x0075,2},           // CAM_AWB_CCM_L_RG_GAIN
	{0xC8CA, 0x011C,2},           // CAM_AWB_CCM_L_BG_GAIN
	{0xC8CC, 0x009A,2},           // CAM_AWB_CCM_M_RG_GAIN
	{0xC8CE, 0x0105,2},           // CAM_AWB_CCM_M_BG_GAIN
	{0xC8D0, 0x00A4,2},           // CAM_AWB_CCM_R_RG_GAIN
	{0xC8D2, 0x00AC,2},           // CAM_AWB_CCM_R_BG_GAIN
	{0xC8D4, 0x0A8C,2},           // CAM_AWB_CCM_L_CTEMP
	{0xC8D6, 0x0F0A,2},           // CAM_AWB_CCM_M_CTEMP
	{0xC8D8, 0x1964,2},           // CAM_AWB_CCM_R_CTEMP

	//[AWB]
	{0xC914, 0x0000,2},           // CAM_STAT_AWB_CLIP_WINDOW_XSTART
	{0xC916, 0x0000,2},           // CAM_STAT_AWB_CLIP_WINDOW_YSTART
	{0xC918, 0x04FF,2},           // CAM_STAT_AWB_CLIP_WINDOW_XEND
	{0xC91A, 0x02CF,2},           // CAM_STAT_AWB_CLIP_WINDOW_YEND
	{0xC904, 0x0033,2},           // CAM_AWB_AWB_XSHIFT_PRE_ADJ
	{0xC906, 0x0040,2},           // CAM_AWB_AWB_YSHIFT_PRE_ADJ
	{0xC8F2, 0x03,1},             // CAM_AWB_AWB_XSCALE
	{0xC8F3, 0x02,1},             // CAM_AWB_AWB_YSCALE
	{0xC906, 0x003C,2},           // CAM_AWB_AWB_YSHIFT_PRE_ADJ
	{0xC8F4, 0x0000,2},           // CAM_AWB_AWB_WEIGHTS_0
	{0xC8F6, 0x0000,2},           // CAM_AWB_AWB_WEIGHTS_1
	{0xC8F8, 0x0000,2},           // CAM_AWB_AWB_WEIGHTS_2
	{0xC8FA, 0xE724,2},           // CAM_AWB_AWB_WEIGHTS_3
	{0xC8FC, 0x1583,2},           // CAM_AWB_AWB_WEIGHTS_4
	{0xC8FE, 0x2045,2},           // CAM_AWB_AWB_WEIGHTS_5
	{0xC900, 0x03FF,2},           // CAM_AWB_AWB_WEIGHTS_6
	{0xC902, 0x007C,2},           // CAM_AWB_AWB_WEIGHTS_7
	{0xC90C, 0x80,1},             // CAM_AWB_K_R_L
	{0xC90D, 0x80,1},             // CAM_AWB_K_G_L
	{0xC90E, 0x80,1},             // CAM_AWB_K_B_L
	{0xC90F, 0x88,1},             // CAM_AWB_K_R_R
	{0xC910, 0x80,1},             // CAM_AWB_K_G_R
	{0xC911, 0x80,1},             // CAM_AWB_K_B_R

	//[Step7-CPIPE_Preference]
	{0xC926, 0x0020,2},           // CAM_LL_START_BRIGHTNESS
	{0xC928, 0x009A,2},           // CAM_LL_STOP_BRIGHTNESS
	{0xC946, 0x0070,2},           // CAM_LL_START_GAIN_METRIC
	{0xC948, 0x00F3,2},           // CAM_LL_STOP_GAIN_METRIC
	{0xC952, 0x0020,2},           // CAM_LL_START_TARGET_LUMA_BM
	{0xC954, 0x009A,2},           // CAM_LL_STOP_TARGET_LUMA_BM
	{0xC92A, 0x80,1},             // CAM_LL_START_SATURATION
	{0xC92B, 0x4B,1},             // CAM_LL_END_SATURATION
	{0xC92C, 0x00,1},             // CAM_LL_START_DESATURATION
	{0xC92D, 0xFF,1},             // CAM_LL_END_DESATURATION
	{0xC92E, 0x3C,1},             // CAM_LL_START_DEMOSAIC
	{0xC92F, 0x02,1},             // CAM_LL_START_AP_GAIN
	{0xC930, 0x06,1},             // CAM_LL_START_AP_THRESH
	{0xC931, 0x64,1},             // CAM_LL_STOP_DEMOSAIC
	{0xC932, 0x01,1},             // CAM_LL_STOP_AP_GAIN
	{0xC933, 0x0C,1},             // CAM_LL_STOP_AP_THRESH
	{0xC934, 0x3C,1},             // CAM_LL_START_NR_RED
	{0xC935, 0x3C,1},             // CAM_LL_START_NR_GREEN
	{0xC936, 0x3C,1},             // CAM_LL_START_NR_BLUE
	{0xC937, 0x0F,1},             // CAM_LL_START_NR_THRESH
	{0xC938, 0x64,1},             // CAM_LL_STOP_NR_RED
	{0xC939, 0x64,1},             // CAM_LL_STOP_NR_GREEN
	{0xC93A, 0x64,1},             // CAM_LL_STOP_NR_BLUE
	{0xC93B, 0x32,1},             // CAM_LL_STOP_NR_THRESH
	{0xC93C, 0x0020,2},           // CAM_LL_START_CONTRAST_BM
	{0xC93E, 0x009A,2},           // CAM_LL_STOP_CONTRAST_BM
	{0xC940, 0x00DC,2},           // CAM_LL_GAMMA
	{0xC942, 0x38,1},             // CAM_LL_START_CONTRAST_GRADIENT
	{0xC943, 0x30,1},             // CAM_LL_STOP_CONTRAST_GRADIENT
	{0xC944, 0x50,1},             // CAM_LL_START_CONTRAST_LUMA_PERCENTAGE
	{0xC945, 0x19,1},             // CAM_LL_STOP_CONTRAST_LUMA_PERCENTAGE
	{0xC94A, 0x0230,2},           // CAM_LL_START_FADE_TO_BLACK_LUMA
	{0xC94C, 0x0010,2},           // CAM_LL_STOP_FADE_TO_BLACK_LUMA
	{0xC94E, 0x01CD,2},           // CAM_LL_CLUSTER_DC_TH_BM
	{0xC950, 0x05,1},             // CAM_LL_CLUSTER_DC_GATE_PERCENTAGE
	{0xC951, 0x40,1},             // CAM_LL_SUMMING_SENSITIVITY_FACTOR
	{0xC87B, 0x1B,1},             // CAM_AET_TARGET_AVERAGE_LUMA_DARK
	{0xC878, 0x0E,1},             // CAM_AET_AEMODE
	{0xC890, 0x0080,2},           // CAM_AET_TARGET_GAIN
	{0xC886, 0x0100,2},           // CAM_AET_AE_MAX_VIRT_AGAIN
	{0xC87C, 0x005A,2},           // CAM_AET_BLACK_CLIPPING_TARGET
	{0xB42A, 0x05,1},             // CCM_DELTA_GAIN
	{0xA80A, 0x20,1},             // AE_TRACK_AE_TRACKING_DAMPENING_SPEED
};

/*static struct msm_camera_i2c_reg_conf mt9m114_groupon_settings[] = {
  {0x104, 0x01},
  };

  static struct msm_camera_i2c_reg_conf mt9m114_groupoff_settings[] = {
  {0x104, 0x00},
  };*/

static struct v4l2_subdev_info mt9m114_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_camera_i2c_conf_array mt9m114_init_conf[] = {
	{&mt9m114_recommend_settings[0],
		ARRAY_SIZE(mt9m114_recommend_settings), 0, MSM_CAMERA_I2C_HYBRID_DATA}
};

static struct msm_sensor_output_info_t mt9m114_dimensions[] = {
	{
		.x_output = 0x500,
		.y_output = 0x2D0,
		.line_length_pclk = 0x500,
		.frame_length_lines = 0x2D0,
		.vt_pixel_clk = 96000000,
		.op_pixel_clk = 96000000,
	},
	{
		.x_output = 0x500,
		.y_output = 0x2D0,
		.line_length_pclk = 0x500,
		.frame_length_lines = 0x2D0,
		.vt_pixel_clk = 96000000,
		.op_pixel_clk = 96000000,
	},
};

static struct msm_camera_csid_vc_cfg mt9m114_cid_cfg[] = {
	{0, 0x1E, CSI_DECODE_8BIT},
	{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
};

static struct msm_camera_csi2_params mt9m114_csi_params = {
	.csid_params = {
		.lane_assign = 0xe4,
		.lane_cnt = 1,
		.lut_params = {
			.num_cid = 2,
			.vc_cfg = mt9m114_cid_cfg,
		},
	},
	.csiphy_params = {
		.lane_cnt = 1,
		.settle_cnt = 7,
	},
};

static struct msm_camera_csi2_params *mt9m114_csi_params_array[] = {
	&mt9m114_csi_params,
	&mt9m114_csi_params,
};

static struct msm_sensor_output_reg_addr_t mt9m114_reg_addr = {
	.x_output = 0xC868,
	.y_output = 0xC86A,
	.line_length_pclk = 0xC868,
	.frame_length_lines = 0xC86A,
};

static struct msm_sensor_id_info_t mt9m114_id_info = {
	.sensor_id_reg_addr = 0x0,
	.sensor_id = 0x2481,
};

static int32_t mt9m114_sensor_ERROR_IF(struct msm_sensor_ctrl_t *s_ctrl,
	unsigned short waddr,uint16_t bfield,
	uint8_t cond, char* errorStr)
{
        int32_t rc = -EIO;
        unsigned short data;
	rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			waddr,&data,MSM_CAMERA_I2C_WORD_DATA);
	if (rc < 0) {
		goto error_if_fail;
	}
	if(cond == 1)
	{
		if(!(data & bfield))
		{
			pr_err("%s\n",errorStr);
			rc = -EINVAL;
		}
	}
	if(cond == 0)
	{
		if(data & bfield)
		{
			pr_err("%s\n",errorStr);
			rc = -EINVAL;
		}
	}

        return rc;
error_if_fail:
        pr_err("ERROR_IF failed\n");
        return rc;
}

static int32_t mt9m114_sensor_POLL_FIELD(struct msm_sensor_ctrl_t *s_ctrl,
	unsigned short waddr,uint16_t bfield, uint8_t cond)
{
        int32_t rc = -EIO;
	int i =0;
	uint16_t data = 0;

	for(i = 100;i>0;i--)
	{
		rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				waddr,&data,MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			goto poll_fail;
		}
                CDBG("POLL_FIELD: data = 0x%x, bfield = 0x%x ...%d\n",data,bfield,i);
		if (cond == 0)
		{
			if(data & bfield)
			{
				msleep(10);
			}
			else
			{
				break;
			}
		}
                if (cond == 1)
                {
                        if(!(data & bfield))
                        {
                                msleep(10);
                        }
                        else
                        {
                                break;
                        }
                }

	}
	if (i <= 0)
		pr_err("POLL_FIELD: timed out\n");
        return rc;
poll_fail:
        pr_err("POLL_FIELD: failed\n");
        return rc;
}
static int32_t mt9m114_sensor_w_BITFIELD(struct msm_sensor_ctrl_t *s_ctrl,
	unsigned short waddr,uint16_t bfield, int flag)
{
        int32_t rc = 0;
        unsigned short data;

        rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			waddr,&data,MSM_CAMERA_I2C_WORD_DATA);
        if (rc < 0) {
                goto bitfield_fail;
        }
	if(flag == 1)
	{
		data = data | bfield;
	}
	else if(flag == 0)
	{
                data = data & ~bfield;
	}
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			waddr,data,MSM_CAMERA_I2C_WORD_DATA);
        if (rc < 0) {
		rc = -EIO;
                goto bitfield_fail;
        }
        return rc;
bitfield_fail:
        pr_err("BITFIELD failed\n");
        return rc;
}

static int32_t mt9m114_sensor_b_BITFIELD(struct msm_sensor_ctrl_t *s_ctrl,
	unsigned short waddr,uint8_t bfield, int flag)
{
        int32_t rc = 0;
        unsigned short data;

        rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
                        waddr,&data,MSM_CAMERA_I2C_BYTE_DATA);
        if (rc < 0) {
                goto bitfield_fail;
        }
        if(flag == 1)
        {
                data = data | bfield;
        }
        else if(flag == 0)
        {
                data = data & ~bfield;
        }
        rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
                        waddr,data,MSM_CAMERA_I2C_BYTE_DATA);
        if (rc < 0) {
		rc = -EIO;
                goto bitfield_fail;
        }
        return rc;
bitfield_fail:
        pr_err("BITFIELD failed\n");
        return rc;
}

static int32_t mt9m114_sensor_setting(struct msm_sensor_ctrl_t *s_ctrl,
		int update_type, int res)
{
	int32_t rc = 0;
	unsigned short data;

	v4l2_subdev_notify(s_ctrl->sensor_v4l2_subdev,
		NOTIFY_ISPIF_STREAM, (void *)ISPIF_STREAM(
		PIX0, ISPIF_OFF_IMMEDIATELY));
	s_ctrl->func_tbl->sensor_stop_stream(s_ctrl);
	msleep(30);

	if (update_type == MSM_SENSOR_REG_INIT) {
		s_ctrl->curr_csi_params = NULL;
		msm_sensor_enable_debugfs(s_ctrl);
		rc = mt9m114_sensor_POLL_FIELD(s_ctrl, COMMAND_REGISTER, HOST_COMMAND_1, 0);
		if (rc < 0)
			return rc;
		/* reset_register_mask_bad */
		rc = mt9m114_sensor_w_BITFIELD(s_ctrl, 0x301A, 0x0200, 1);
		if (rc < 0)
			return rc;
		msm_sensor_write_init_settings(s_ctrl);
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x3C44, 0x0080,MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0)
			return rc;
		/* MIPI non-continuous clock mode */
		rc = mt9m114_sensor_w_BITFIELD(s_ctrl, 0x3C40, 0x0004, 0);
		if (rc < 0)
			return rc;
		// Speed up AE/AWB
		rc = mt9m114_sensor_b_BITFIELD(s_ctrl, 0xA802, 0x08, 1);
		if (rc < 0)
			return rc;
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0xC908, 0x01,MSM_CAMERA_I2C_BYTE_DATA);
		if (rc < 0)
			return rc;
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0xC879, 0x01,MSM_CAMERA_I2C_BYTE_DATA);
		if (rc < 0)
			return rc;
		rc = mt9m114_sensor_b_BITFIELD(s_ctrl, 0xC909, 0x01, 0);
		if (rc < 0)
			return rc;
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0xA80A, 0x18,MSM_CAMERA_I2C_BYTE_DATA);
		if (rc < 0)
			return rc;
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0xA80B, 0x18,MSM_CAMERA_I2C_BYTE_DATA);
		if (rc < 0)
			return rc;
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0xAC16, 0x18,MSM_CAMERA_I2C_BYTE_DATA);
		if (rc < 0)
			return rc;
		rc = mt9m114_sensor_b_BITFIELD(s_ctrl, 0xC878, 0x08, 1);
		if (rc < 0)
			return rc;

		// disables fade-to-black
		rc = mt9m114_sensor_b_BITFIELD(s_ctrl, 0xBC02, 0x08, 0);
		if (rc < 0)
			return rc;
	} else if (update_type == MSM_SENSOR_UPDATE_PERIODIC) {
		if (s_ctrl->curr_csi_params != s_ctrl->csi_params[res]) {
			s_ctrl->curr_csi_params = s_ctrl->csi_params[res];
			v4l2_subdev_notify(s_ctrl->sensor_v4l2_subdev,
				NOTIFY_CSID_CFG,
				&s_ctrl->curr_csi_params->csid_params);
			v4l2_subdev_notify(s_ctrl->sensor_v4l2_subdev,
						NOTIFY_CID_CHANGE, NULL);
			mb();
			v4l2_subdev_notify(s_ctrl->sensor_v4l2_subdev,
				NOTIFY_CSIPHY_CFG,
				&s_ctrl->curr_csi_params->csiphy_params);
			mb();
			msleep(20);
		}

		v4l2_subdev_notify(s_ctrl->sensor_v4l2_subdev,
			NOTIFY_PCLK_CHANGE, &s_ctrl->msm_sensor_reg->
			output_settings[res].op_pixel_clk);
		v4l2_subdev_notify(s_ctrl->sensor_v4l2_subdev,
			NOTIFY_ISPIF_STREAM, (void *)ISPIF_STREAM(
			PIX0, ISPIF_ON_FRAME_BOUNDARY));
		s_ctrl->func_tbl->sensor_start_stream(s_ctrl);
		msleep(30);
		rc = mt9m114_sensor_ERROR_IF(s_ctrl, COMMAND_REGISTER, HOST_COMMAND_1,
				0,"Set State cmd bit is already set");
		if (rc < 0)
			return rc;
		rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, COMMAND_REGISTER,
				(HOST_COMMAND_OK | HOST_COMMAND_1),
				MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0)
			return rc;
		rc = mt9m114_sensor_POLL_FIELD(s_ctrl, COMMAND_REGISTER, HOST_COMMAND_1, 0);
		if (rc < 0)
			return rc;
		rc = mt9m114_sensor_ERROR_IF(s_ctrl, COMMAND_REGISTER, HOST_COMMAND_OK,
				1, "Set State cmd failed");
		if (rc < 0)
			return rc;
		/* Check if the sensor is streaming */
		rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				0xDC01,&data,MSM_CAMERA_I2C_BYTE_DATA);
		if (rc < 0)
			return rc;
		if (data == 0x31)
			pr_err("Sensor is streaming!\n");
		else
			pr_err("Sensor is NOT streaming!\n");
		return rc;
	}
	return rc;
}

static int mt9m114_sensor_config(void __user *argp)
{
	return msm_sensor_config(&mt9m114_s_ctrl, argp);
}


static int mt9m114_sensor_open_init(const struct msm_camera_sensor_info *data)
{
	return msm_sensor_open_init(&mt9m114_s_ctrl, data);
}

static int32_t mt9m114_power_on(const struct msm_camera_sensor_info *data)
{
	int32_t rc = 0;

	CDBG("mt9m114_power_on\n");
	mutex_lock(&mt9m114_mut);

	// obtain gpios
	rc = gpio_request(CAM2_DIGITAL_EN_N, "mt9m114");
	if (rc) {
		pr_err("mt9m114: gpio request CAM2_DIGITAL_EN_N failed (%d)\n",
				rc);
		goto power_on_done;
	}
	rc = gpio_request(CAM2_ANALOG_EN, "mt9m114");
	if (rc) {
		pr_err("mt9m114: gpio request CAM2_ANALOG_EN failed (%d)\n",rc);
		goto power_on_done;
	}
	rc = gpio_request(CAM2_RESET, "mt9m114");
	if (rc) {
		pr_err("mt9m114: gpio request CAM2_RESET failed (%d)\n",rc);
		goto power_on_done;
	}

	// turn on digital supply
	gpio_direction_output(CAM2_DIGITAL_EN_N, 0);

	// turn on analog supply
	gpio_direction_output(CAM2_ANALOG_EN, 1);

	// turn on mclk
	msm_camio_clk_rate_set(MT9M114_DEFAULT_MASTER_CLK_RATE);
	usleep_range(1000, 2000);

	// toggle reset
	gpio_direction_output(CAM2_RESET, 0);
	usleep_range(5000, 6000);
	gpio_set_value_cansleep(CAM2_RESET, 1);
	msleep(50);

power_on_done:
	mutex_unlock(&mt9m114_mut);
	return rc;
}

static int32_t mt9m114_power_off(const struct msm_camera_sensor_info *data)
{
	CDBG("mt9m114_power_off\n");
	mutex_lock(&mt9m114_mut);

	// FIXME: do this?
	//mt9m114_i2c_write_b_sensor(REG_MODE_SELECT,
	//		MODE_SELECT_STANDBY_MODE);
	//msleep(10);

	// assert reset
	gpio_direction_output(CAM2_RESET, 0);

	// turn off mclk
	// FIXME: might be done outside this driver

	// turn off analog supply
	gpio_direction_output(CAM2_ANALOG_EN, 0);

	// turn off digital supply
	gpio_direction_output(CAM2_DIGITAL_EN_N, 1);

	// free gpios
	gpio_free(CAM2_RESET);
	gpio_free(CAM2_ANALOG_EN);
	gpio_free(CAM2_DIGITAL_EN_N);

	mutex_unlock(&mt9m114_mut);
	return 0;
}

static int mt9m114_sensor_release(void)
{
	CDBG("%s\n", __func__);
	return mt9m114_power_off(mt9m114_s_ctrl.sensordata);
}

static int32_t mt9m114_match_id(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	uint16_t chipid = 0;
	rc = msm_camera_i2c_read(
			s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_id_info->sensor_id_reg_addr, &chipid,
			MSM_CAMERA_I2C_WORD_DATA);
	if (rc < 0) {
		pr_err("%s: read id failed\n", __func__);
		return rc;
	}

	printk("mt9m114 chipid: %04x\n", chipid);
	if (chipid != s_ctrl->sensor_id_info->sensor_id) {
		pr_err("%s: chip id does not match\n", __func__);
		return -ENODEV;
	}
	return rc;
}

static const struct i2c_device_id mt9m114_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&mt9m114_s_ctrl},
	{ }
};

static struct i2c_driver mt9m114_i2c_driver = {
	.id_table = mt9m114_i2c_id,
	.probe  = msm_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client mt9m114_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};


static int mt9m114_sensor_v4l2_probe(const struct msm_camera_sensor_info *info,
		struct v4l2_subdev *sdev, struct msm_sensor_ctrl *s)
{
	return msm_sensor_v4l2_probe(&mt9m114_s_ctrl, info, sdev, s);
}

static int mt9m114_probe(struct platform_device *pdev)
{
	return msm_sensor_register(pdev, mt9m114_sensor_v4l2_probe);
}

struct platform_driver mt9m114_driver = {
	.probe = mt9m114_probe,
	.driver = {
		.name = PLATFORM_DRIVER_NAME,
		.owner = THIS_MODULE,
	},
};

static int __init mt9m114_init_module(void)
{
	return platform_driver_register(&mt9m114_driver);
}

static struct v4l2_subdev_core_ops mt9m114_subdev_core_ops;
static struct v4l2_subdev_video_ops mt9m114_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops mt9m114_subdev_ops = {
	.core = &mt9m114_subdev_core_ops,
	.video  = &mt9m114_subdev_video_ops,
};

static struct msm_sensor_fn_t mt9m114_func_tbl = {
	.sensor_start_stream    = msm_sensor_start_stream,
	.sensor_stop_stream     = msm_sensor_stop_stream,
	//	.sensor_group_hold_on = msm_sensor_group_hold_on,
	//	.sensor_group_hold_off = msm_sensor_group_hold_off,
	//	.sensor_get_prev_lines_pf = msm_sensor_get_prev_lines_pf,
	//	.sensor_get_prev_pixels_pl = msm_sensor_get_prev_pixels_pl,
	//	.sensor_get_pict_lines_pf = msm_sensor_get_pict_lines_pf,
	//	.sensor_get_pict_pixels_pl = msm_sensor_get_pict_pixels_pl,
	//	.sensor_get_pict_max_exp_lc = msm_sensor_get_pict_max_exp_lc,
	//	.sensor_get_pict_fps = msm_sensor_get_pict_fps,
	//	.sensor_set_fps = msm_sensor_set_fps,
	//	.sensor_write_exp_gain = msm_sensor_write_exp_gain1,
	.sensor_setting         = mt9m114_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init       = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config          = mt9m114_sensor_config,
	.sensor_open_init       = mt9m114_sensor_open_init,
	.sensor_release         = mt9m114_sensor_release,
	.sensor_power_up        = mt9m114_power_on,
	.sensor_power_down      = mt9m114_power_off,
	.sensor_match_id        = mt9m114_match_id,
	.sensor_probe           = msm_sensor_probe,
};

static struct msm_sensor_reg_t mt9m114_regs = {
	.default_data_type      = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf      = mt9m114_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(mt9m114_start_settings),
	.stop_stream_conf       = mt9m114_stop_settings,
	.stop_stream_conf_size  = ARRAY_SIZE(mt9m114_stop_settings),
	//	.group_hold_on_conf = mt9m114_groupon_settings,
	//	.group_hold_on_conf_size = ARRAY_SIZE(mt9m114_groupon_settings),
	//	.group_hold_off_conf = mt9m114_groupoff_settings,
	//	.group_hold_off_conf_size =
	//		ARRAY_SIZE(mt9m114_groupoff_settings),
	.init_settings          = &mt9m114_init_conf[0],
	.init_size              = ARRAY_SIZE(mt9m114_init_conf),
	//	.mode_settings = &mt9m114_confs[0],
	.output_settings        = &mt9m114_dimensions[0],
	//	.num_conf = ARRAY_SIZE(mt9m114_confs),
	.num_conf               = 2,
};

static struct msm_sensor_ctrl_t mt9m114_s_ctrl = {
	.msm_sensor_reg               = &mt9m114_regs,
	.sensor_i2c_client            = &mt9m114_sensor_i2c_client,
	.sensor_i2c_addr              = 0x90,
	//	.sensor_eeprom_client = &mt9m114_eeprom_client,
	.sensor_output_reg_addr       = &mt9m114_reg_addr,
	.sensor_id_info               = &mt9m114_id_info,
	//	.sensor_exp_gain_info = &mt9m114_exp_gain_info,
	.cam_mode                     = MSM_SENSOR_MODE_INVALID,
	.csi_params                   = &mt9m114_csi_params_array[0],
	.msm_sensor_mutex             = &mt9m114_mut,
	.sensor_i2c_driver            = &mt9m114_i2c_driver,
	.sensor_v4l2_subdev_info      = mt9m114_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(mt9m114_subdev_info),
	.sensor_v4l2_subdev_ops       = &mt9m114_subdev_ops,
	.func_tbl                     = &mt9m114_func_tbl,
};

module_init(mt9m114_init_module);
MODULE_DESCRIPTION("Aptina 1.26MP YUV sensor driver");
MODULE_LICENSE("GPL v2");
