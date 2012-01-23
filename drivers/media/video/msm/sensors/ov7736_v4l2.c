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
#include <linux/regulator/consumer.h>

#define SENSOR_NAME "ov7736"

/*=============================================================
  SENSOR REGISTER DEFINES
  ==============================================================*/
#define OV7736_DEFAULT_CLOCK_RATE	24000000

#define CAM2_RESET 76
#define CAM2_ANALOG_EN 82
#define CAM2_DIGITAL_EN_N 89

DEFINE_MUTEX(ov7736_mut);
static struct msm_sensor_ctrl_t ov7736_s_ctrl;

static struct regulator *reg_1p8;

/*=============================================================
  SENSOR REGISTER DEFINES
  ==============================================================*/

static struct msm_camera_i2c_reg_conf ov7736_start_settings[] = {
	{0x3008, 0x02},
	{0xFFFE, 200}, /* delay */
};

static struct msm_camera_i2c_reg_conf ov7736_stop_settings[] = {
	{0x3008, 0x42},
	{0xFFFE, 100}, /* delay */
};

static struct msm_camera_i2c_conf_array ov7736_init_conf[] = {
};

static struct msm_camera_i2c_reg_conf mode_preview_tbl[] = {
	{0x3008, 0x82},
	{0xFFFE, 200}, /* delay */
	{0x3008, 0x42},
	{0xFFFE, 100}, /* delay */
	{0x3630, 0x11},
	{0x3104, 0x03},
	{0x3017, 0x7f},
	{0x3018, 0xfc},
	{0x3600, 0x1c},
	{0x3602, 0x04},
	{0x3611, 0x44},
	{0x3612, 0x63},
	{0x3631, 0x22},
	{0x3622, 0x00},
	{0x3633, 0x25},
	{0x370d, 0x07},
	{0x3620, 0x42},
	{0x3714, 0x19},
	{0x3715, 0xfa},
	{0x370b, 0x43},
	{0x3713, 0x1a},
	{0x401c, 0x00},
	{0x401e, 0x11},
	{0x4702, 0x01},
	{0x3a00, 0x7a},
	{0x3a18, 0x00},
	{0x3a19, 0x3f},
	{0x300f, 0x88},
	{0x3011, 0x08},
	{0x4303, 0xff},
	{0x4307, 0xff},
	{0x430b, 0xff},
	{0x4305, 0x00},
	{0x4309, 0x00},
	{0x430d, 0x00},
	{0x5181, 0x04},
	{0x5481, 0x26},
	{0x5482, 0x35},
	{0x5483, 0x48},
	{0x5484, 0x63},
	{0x5485, 0x6e},
	{0x5486, 0x77},
	{0x5487, 0x80},
	{0x5488, 0x88},
	{0x5489, 0x8f},
	{0x548a, 0x96},
	{0x548b, 0xa3},
	{0x548c, 0xaf},
	{0x548d, 0xc5},
	{0x548e, 0xd7},
	{0x548f, 0xe8},
	{0x5490, 0x0f},
	{0x4001, 0x02},
	{0x4004, 0x06},
	{0x3800, 0x00},
	{0x3801, 0x8e},
	{0x3810, 0x08},
	{0x3811, 0x02},
	{0x380c, 0x03},
	{0x380d, 0x20},
	{0x380e, 0x01},
	{0x380f, 0xf4},
	{0x3a09, 0x96},
	{0x3a0b, 0x7d},
	{0x4300, 0x30},
	{0x501f, 0x01},
	{0x5000, 0x4f},
	{0x5001, 0x47},
	{0x370d, 0x0b},
	{0x3017, 0x00},
	{0x3018, 0x00},
	{0x300e, 0x04},
	{0x4801, 0x0f},
	{0x4601, 0x02},
	{0x300f, 0x8a},
	{0x3011, 0x02},
	{0x302d, 0x50},
	{0x3632, 0x39},
	{0x3010, 0x10},
	{0x4300, 0x3f},
	{0x3715, 0x1a},
	{0x370e, 0x00},
	{0x3713, 0x08},
	{0x3703, 0x2c},
	{0x3620, 0xc2},
	{0x3714, 0x36},
	{0x3716, 0x01},
	{0x3623, 0x03},
	{0x3c00, 0x00},
	{0x3c01, 0x32},
	{0x3c04, 0x12},
	{0x3c05, 0x60},
	{0x3c06, 0x00},
	{0x3c07, 0x20},
	{0x3c08, 0x00},
	{0x3c09, 0xc2},
	{0x300d, 0x22},
	{0x3c0a, 0x9c},
	{0x3c0b, 0x40},
	{0x5180, 0x02},
	{0x5181, 0x02},
	{0x3a0f, 0x35},
	{0x3a10, 0x2c},
	{0x3a1b, 0x36},
	{0x3a1e, 0x2d},
	{0x3a11, 0x90},
	{0x3a1f, 0x10},
	{0x5000, 0xcf},
	{0x5481, 0x0a},
	{0x5482, 0x13},
	{0x5483, 0x23},
	{0x5484, 0x40},
	{0x5485, 0x4d},
	{0x5486, 0x58},
	{0x5487, 0x64},
	{0x5488, 0x6e},
	{0x5489, 0x78},
	{0x548a, 0x81},
	{0x548b, 0x92},
	{0x548c, 0xa1},
	{0x548d, 0xbb},
	{0x548e, 0xcf},
	{0x548f, 0xe3},
	{0x5490, 0x26},
	{0x5380, 0x42},
	{0x5381, 0x33},
	{0x5382, 0x0f},
	{0x5383, 0x0b},
	{0x5384, 0x42},
	{0x5385, 0x4d},
	{0x5392, 0x1e},
	{0x5801, 0x00},
	{0x5802, 0x00},
	{0x5803, 0x00},
	{0x5804, 0x12},
	{0x5805, 0x15},
	{0x5806, 0x08},
	{0x5001, 0xc7},
	{0x5580, 0x02},
	{0x5583, 0x40},
	{0x5584, 0x26},
	{0x5589, 0x10},
	{0x558a, 0x00},
	{0x558b, 0x3e},
	{0x5300, 0x0f},
	{0x5301, 0x30},
	{0x5302, 0x0d},
	{0x5303, 0x02},
	{0x5300, 0x0f},
	{0x5301, 0x30},
	{0x5302, 0x0d},
	{0x5303, 0x02},
	{0x5304, 0x0e},
	{0x5305, 0x30},
	{0x5306, 0x06},
	{0x5307, 0x40},
	{0x5680, 0x00},
	{0x5681, 0x50},
	{0x5682, 0x00},
	{0x5683, 0x3c},
	{0x5684, 0x11},
	{0x5685, 0xe0},
	{0x5686, 0x0d},
	{0x5687, 0x68},
	{0x5688, 0x03},
};

static struct msm_camera_i2c_reg_conf const mode_snapshot_tbl[] = {
};

static struct msm_camera_i2c_reg_conf const lensrolloff_tbl[] = {
};

static struct msm_camera_i2c_reg_conf const mode_tp_disable_tbl[] = {
	{0x503D, 0x00}
};

static struct msm_camera_i2c_reg_conf const mode_tp_colorbars_tbl[] = {
	{ 0x3503, 0x03},
	{ 0x350a, 0x00},
	{ 0x350a, 0x10},
	{ 0x3500, 0x00},
	{ 0x3501, 0x04},
	{ 0x3502, 0xf0},
	{ 0x504e, 0x04},
	{ 0x504f, 0x00},
	{ 0x5050, 0x04},
	{ 0x5051, 0x00},
	{ 0x5052, 0x04},
	{ 0x5053, 0x00},
	{ 0x5186, 0x03},
	{ 0x5000, 0x4f},
	{ 0x503D, 0x80}
};

static struct msm_camera_i2c_reg_conf const mode_tp_checkerboard_tbl[] = {
};

static struct msm_camera_i2c_reg_conf const effects_mono_tbl[] = {
	{ 0x5001, 0x87},
	{ 0x5580, 0x22},
	{ 0x5583, 0x80},
	{ 0x5584, 0x80}
};

static struct msm_camera_i2c_reg_conf const effects_negative_tbl[] = {
	{ 0x5001, 0x87},
	{ 0x5580, 0x42},
	{ 0x5583, 0x80},
	{ 0x5584, 0x80}
};

static struct msm_camera_i2c_reg_conf const effects_sepia_tbl[] = {
	{ 0x5001, 0x87},
	{ 0x5580, 0x1a},
	{ 0x5583, 0x40},
	{ 0x5584, 0xa0}
};

static struct msm_camera_i2c_reg_conf const effects_redtint_tbl[] = {
	{ 0x5001, 0x87},
	{ 0x5580, 0x1a},
	{ 0x5583, 0x80},
	{ 0x5584, 0xc0}
};

static struct msm_camera_i2c_reg_conf const effects_bluetint_tbl[] = {
	{ 0x5001, 0x87},
	{ 0x5580, 0x1a},
	{ 0x5583, 0xa0},
	{ 0x5584, 0x40}
};

static struct msm_camera_i2c_reg_conf const effects_greentint_tbl[] = {
	{ 0x5001, 0x87},
	{ 0x5580, 0x1a},
	{ 0x5583, 0x60},
	{ 0x5584, 0x60}
};

static struct msm_camera_i2c_reg_conf const effects_off_tbl[] = {
	{ 0x5001, 0x07},
	{ 0x5580, 0x02},
	{ 0x5583, 0x40},
	{ 0x5584, 0x40}
};

static struct msm_camera_i2c_conf_array ov7736_confs[] = {
	{&mode_preview_tbl[0],
		ARRAY_SIZE(mode_preview_tbl), 50, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct v4l2_subdev_info ov7736_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_sensor_output_info_t ov7736_dimensions[] = {
	{
		.x_output = 640,
		.y_output = 480,
		.line_length_pclk = 640,
		.frame_length_lines = 480,
		.vt_pixel_clk = 200000000,
		.op_pixel_clk = 200000000,
	},
	{
		.x_output = 640,
		.y_output = 480,
		.line_length_pclk = 640,
		.frame_length_lines = 480,
		.vt_pixel_clk = 200000000,
		.op_pixel_clk = 200000000,
	},
};

static struct msm_camera_csid_vc_cfg ov7736_cid_cfg[] = {
	{0, 0x1E, CSI_DECODE_8BIT},
	{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
};

static struct msm_camera_csi2_params ov7736_csi_params = {
	.csid_params = {
		.lane_assign = 0xe4,
		.lane_cnt = 1,
		.lut_params = {
			.num_cid = 2,
			.vc_cfg = ov7736_cid_cfg,
		},
	},
	.csiphy_params = {
		.lane_cnt = 1,
		.settle_cnt = 8,
	},
};

static struct msm_camera_csi2_params *ov7736_csi_params_array[] = {
	&ov7736_csi_params,
	&ov7736_csi_params,
};

static struct msm_sensor_output_reg_addr_t ov7736_reg_addr = {
	.x_output = 0xFFFF,
	.y_output = 0xFFFF,
	.line_length_pclk = 0xFFFF,
	.frame_length_lines = 0xFFFF,
};

static struct msm_sensor_id_info_t ov7736_id_info = {
	.sensor_id_reg_addr = 0x300A,
	.sensor_id = 0x7736,
};

static int32_t ov7736_regulator_on(struct regulator **reg, char *regname,
		int uV)
{
	int32_t rc = 0;
	pr_info("ov7736_regulator_on: %s %d\n", regname, uV);

	*reg = regulator_get(NULL, regname);
	if (IS_ERR(*reg)) {
		pr_err("ov7736: failed to get %s (%ld)\n",
				regname, PTR_ERR(*reg));
		goto reg_on_done;
	}
	rc = regulator_set_voltage(*reg, uV, uV);
	if (rc) {
		pr_err("ov7736: failed to set voltage for %s (%d)\n",
				regname, rc);
		goto reg_on_done;
	}
	rc = regulator_enable(*reg);
	if (rc) {
		pr_err("ov7736: failed to enable %s (%d)\n",
				regname, rc);
		goto reg_on_done;
	}
reg_on_done:
	return rc;
}

static int32_t ov7736_regulator_off(struct regulator *reg, char *regname)
{
	int32_t rc = 0;

	if (reg) {
		pr_info("ov7736_regulator_off: %s\n", regname);

		rc = regulator_disable(reg);
		if (rc) {
			pr_err("ov7736: failed to disable %s (%d)\n",
					regname, rc);
			goto reg_off_done;
		}
		regulator_put(reg);
	}
reg_off_done:
	return rc;
}

static int32_t ov7736_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;

	pr_info("ov7736_power_on\n");

	rc = gpio_request(CAM2_ANALOG_EN, "ov7736");
	if (rc < 0) {
		pr_err("ov7736: gpio request CAM1_ANALOG_ENABLE failed (%d)\n",
				rc);
		goto power_on_done;
	}
	rc = gpio_request(CAM2_DIGITAL_EN_N/*CAM2_PWRDWN*/, "ov7736");
	if (rc < 0) {
		pr_err("ov7736: gpio request CAM1_PWRDWN failed (%d)\n", rc);
		goto power_on_done;
	}
	rc = gpio_request(CAM2_RESET, "ov7736");
	if (rc < 0) {
		pr_err("7736: gpio request CAM1_RESET failed (%d)\n", rc);
		goto power_on_done;
	}
	gpio_direction_output(CAM2_DIGITAL_EN_N, 1);

	rc = ov7736_regulator_on(&reg_1p8, "8921_l29", 1800000);
	if (rc < 0)
		goto power_on_done;
	usleep_range(5000, 5000);

	pr_info("ov7736 enabling AVDD\n");
	gpio_direction_output(CAM2_ANALOG_EN, 1);
	usleep(20000);

	pr_info("ov7736 enabling MCLK\n");
	msm_sensor_probe_on(&s_ctrl->sensor_i2c_client->client->dev);
	msm_camio_clk_rate_set(OV7736_DEFAULT_CLOCK_RATE);
	usleep(5000);

	pr_info("ov7736 setting PWRDWN Low\n");
	gpio_direction_output(CAM2_DIGITAL_EN_N, 0);

	pr_info("ov7736 setting Reset High\n");
	gpio_direction_output(CAM2_RESET, 1);
	msleep(20);

	pr_info("ov7736 power up sequence complete\n");
power_on_done:
	return rc;
}


static int32_t ov7736_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	pr_info("ov7736_sensor_power_off\n");

	/* Set Reset Low */
	gpio_direction_output(CAM2_RESET, 0);

	/* Disable AVDD */
	gpio_direction_output(CAM2_ANALOG_EN, 0);

	/* Set PWRDWN Low */
	gpio_direction_output(CAM2_DIGITAL_EN_N, 0);

	/* Clean up */
	gpio_free(CAM2_DIGITAL_EN_N);
	gpio_free(CAM2_RESET);
	gpio_free(CAM2_ANALOG_EN);
	ov7736_regulator_off(reg_1p8, "8921_l29");

	msm_sensor_probe_off(&s_ctrl->sensor_i2c_client->client->dev);

	return 0;
}

static int32_t ov7736_sensor_match_id(struct msm_sensor_ctrl_t *s_ctrl)
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

	pr_info("ov7736 chipid: %04x\n", chipid);
	if (chipid != s_ctrl->sensor_id_info->sensor_id) {
		pr_err("%s: chip id does not match\n", __func__);
		return -ENODEV;
	}
	pr_info("ov7736: match_id success\n");
	return 0;
}
static const struct i2c_device_id ov7736_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&ov7736_s_ctrl},
	{ }
};

static struct i2c_driver ov7736_i2c_driver = {
	.id_table = ov7736_i2c_id,
	.probe  = msm_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ov7736_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};


static int __init ov7736_init_module(void)
{
	return i2c_add_driver(&ov7736_i2c_driver);
}

static struct v4l2_subdev_core_ops ov7736_subdev_core_ops = {
	.s_ctrl = msm_sensor_v4l2_s_ctrl,
	.queryctrl = msm_sensor_v4l2_query_ctrl,
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops ov7736_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops ov7736_subdev_ops = {
	.core = &ov7736_subdev_core_ops,
	.video  = &ov7736_subdev_video_ops,
};

static struct msm_sensor_fn_t ov7736_func_tbl = {
	.sensor_start_stream    = msm_sensor_start_stream,
	.sensor_stop_stream     = msm_sensor_stop_stream,
	.sensor_setting         = msm_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init       = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config          = msm_sensor_config,
	.sensor_power_up        = ov7736_sensor_power_up,
	.sensor_power_down      = ov7736_sensor_power_down,
	.sensor_match_id        = ov7736_sensor_match_id,
};

static struct msm_sensor_reg_t ov7736_regs = {
	.default_data_type      = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf      = ov7736_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(ov7736_start_settings),
	.stop_stream_conf       = ov7736_stop_settings,
	.stop_stream_conf_size  = ARRAY_SIZE(ov7736_stop_settings),
	.init_settings          = &ov7736_init_conf[0],
	.init_size              = ARRAY_SIZE(ov7736_init_conf),
	.output_settings        = &ov7736_dimensions[0],
	.mode_settings          = &ov7736_confs[0],
	.num_conf               = ARRAY_SIZE(ov7736_confs),
};

static struct msm_sensor_ctrl_t ov7736_s_ctrl = {
	.msm_sensor_reg               = &ov7736_regs,
	.sensor_i2c_client            = &ov7736_sensor_i2c_client,
	.sensor_i2c_addr              = 0x78,

	.sensor_output_reg_addr       = &ov7736_reg_addr,
	.sensor_id_info               = &ov7736_id_info,

	.cam_mode                     = MSM_SENSOR_MODE_INVALID,
	.csi_params                   = &ov7736_csi_params_array[0],
	.msm_sensor_mutex             = &ov7736_mut,
	.sensor_i2c_driver            = &ov7736_i2c_driver,
	.sensor_v4l2_subdev_info      = ov7736_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ov7736_subdev_info),
	.sensor_v4l2_subdev_ops       = &ov7736_subdev_ops,
	.func_tbl                     = &ov7736_func_tbl,
};

module_init(ov7736_init_module);
MODULE_DESCRIPTION("OMNIVISION VGA YUV sensor driver");
MODULE_LICENSE("GPL v2");
