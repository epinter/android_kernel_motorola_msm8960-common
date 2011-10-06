/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
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

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_mot.h"
#include "mdp4.h"

#define DCS_CMD_SOFT_RESET           0x01
#define DCS_CMD_GET_POWER_MODE       0x0A
#define DCS_CMD_ENTER_SLEEP_MODE     0x10
#define DCS_CMD_EXIT_SLEEP_MODE      0x11
#define DCS_CMD_SET_DISPLAY_ON       0x29
#define DCS_CMD_SET_DISPLAY_OFF      0x28
#define DCS_CMD_SET_COLUMN_ADDRESS   0x2A
#define DCS_CMD_SET_PAGE_ADDRESS     0x2B
#define DCS_CMD_SET_TEAR_OFF         0x34
#define DCS_CMD_SET_TEAR_ON          0x35
#define DCS_CMD_SET_TEAR_SCANLINE    0x44
#define DCS_CMD_SET_BRIGHTNESS       0x51
#define DCS_CMD_READ_BRIGHTNESS      0x52
#define DCS_CMD_SET_CTRL_DISP        0x53
#define DCS_CMD_READ_CTRL_DISP       0x54
#define DCS_CMD_SET_CABC             0x55
#define DCS_CMD_READ_CABC            0x56
#define DCS_CMD_READ_DDB_START       0xA1
#define DCS_CMD_SET_MCS              0xB2
#define DCS_CMD_SET_DISPLAY_MODE     0xB3
#define DCS_CMD_SET_BCKLGHT_PWM      0xB4
#define DCS_CMD_DATA_LANE_CONFIG     0xB5
#define DCS_CMD_READ_DA              0xDA
#define DCS_CMD_READ_DB              0xDB
#define DCS_CMD_READ_DC              0xDC
#define DCS_CMD_RDDSDR               0x0F

#define INVALID_VALUE   0xFFFF

static struct mipi_dsi_panel_platform_data *mipi_mot_pdata;

static struct dsi_buf mot_tx_buf;
static struct dsi_buf mot_rx_buf;

static char sw_reset[2] = {DCS_CMD_SOFT_RESET, 0x00};
static char enter_sleep[2] = {DCS_CMD_ENTER_SLEEP_MODE, 0x00};
static char exit_sleep[2] = {DCS_CMD_EXIT_SLEEP_MODE, 0x00};
static char display_off[2] = {DCS_CMD_SET_DISPLAY_OFF, 0x00};
static char display_on[2] = {DCS_CMD_SET_DISPLAY_ON, 0x00};

static char led_pwm1[2] = {DCS_CMD_SET_BRIGHTNESS, 0xFF};
static char led_pwm2[2] = {DCS_CMD_SET_CTRL_DISP, 0x2C};
/* static char led_pwm3[2] = {DCS_CMD_SET_CABC, 0x00}; */

static struct dsi_cmd_desc mot_cmd_backlight_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(led_pwm1), led_pwm1},
};

static struct dsi_cmd_desc mot_video_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 50,
		sizeof(sw_reset), sw_reset},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc mot_cmd_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 20,
		sizeof(sw_reset), sw_reset},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,
		sizeof(led_pwm1), led_pwm1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1,
		sizeof(led_pwm2), led_pwm2},
	{DTYPE_DCS_WRITE, 1, 0, 0, 1,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc mot_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(enter_sleep), enter_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 1,
		sizeof(display_off), display_off},
};

static char manufacture_id[2] = {0xda, 0x00}; /* DTYPE_DCS_READ */
static char controller_ver[2] = {0xdb, 0x00};
static char controller_drv_ver[2] = {0xdc, 0x00};

static struct dsi_cmd_desc mot_manufacture_id_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(manufacture_id), manufacture_id};

static struct dsi_cmd_desc mot_controller_ver_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(controller_ver), controller_ver};

static struct dsi_cmd_desc mot_controller_drv_ver_cmd = {
        DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(controller_drv_ver),
	controller_drv_ver};


static uint32 mipi_mot_get_panel_info(struct msm_fb_data_type *mfd,
					struct dsi_cmd_desc *cmd)
{
	struct dsi_buf *rp, *tp;
	uint32 *lp;

	tp = &mot_tx_buf;
	rp = &mot_rx_buf;
	mipi_dsi_buf_init(rp);
	mipi_dsi_buf_init(tp);

	mipi_dsi_cmds_rx(mfd, tp, rp, cmd, 1);
	lp = (uint32 *)rp->data;

	return *lp;
}

static u16 mipi_mot_manufacture_id(struct msm_fb_data_type *mfd)
{
	struct dsi_cmd_desc *cmd;
	static u16 manufacture_id = INVALID_VALUE;

	if (manufacture_id == INVALID_VALUE) {
		cmd = &mot_manufacture_id_cmd;
		manufacture_id = mipi_mot_get_panel_info(mfd, cmd);
		pr_info(" MIPI panel Manufacture_id = 0x%x \n", manufacture_id);
	}

	return manufacture_id;
}

static u16 mipi_mot_controller_ver(struct msm_fb_data_type *mfd)
{
	struct dsi_cmd_desc *cmd;
	static u16 controller_ver = INVALID_VALUE;

	if (controller_ver == INVALID_VALUE) {
		cmd = &mot_controller_ver_cmd;
		controller_ver =  mipi_mot_get_panel_info(mfd, cmd);
		pr_info(" MIPI panel Controller_ver = 0x%x \n", controller_ver);
	}

	return controller_ver;
}


static u16 mipi_mot_controller_drv_ver(struct msm_fb_data_type *mfd)
{
        struct dsi_cmd_desc *cmd;
	static u16 controller_drv_ver = INVALID_VALUE;

	if (controller_drv_ver == INVALID_VALUE) {
		cmd = &mot_controller_drv_ver_cmd;
		controller_drv_ver = mipi_mot_get_panel_info(mfd, cmd);
		pr_info(" MIPI panel Controller_drv_ver = 0x%x \n",
							controller_drv_ver);
	}

        return controller_drv_ver;
}

static int mipi_mot_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;

	pr_info("%s is called \n", __func__);

	mfd = platform_get_drvdata(pdev);
	mipi  = &mfd->panel_info.mipi;

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	if (mipi->mode == DSI_VIDEO_MODE) {
		mipi_dsi_cmds_tx(mfd, &mot_tx_buf, mot_video_on_cmds,
			ARRAY_SIZE(mot_video_on_cmds));
	} else {
		mipi_dsi_cmds_tx(mfd, &mot_tx_buf, mot_cmd_on_cmds,
			ARRAY_SIZE(mot_cmd_on_cmds));

		mipi_dsi_cmd_bta_sw_trigger(); /* clean up ack_err_status */

		mipi_mot_manufacture_id(mfd);
		mipi_mot_controller_ver(mfd);
		mipi_mot_controller_drv_ver(mfd);
	}

	return 0;
}

static int mipi_mot_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	pr_info("%s is called \n", __func__);

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi_dsi_cmds_tx(mfd, &mot_tx_buf, mot_display_off_cmds,
			ARRAY_SIZE(mot_display_off_cmds));

	return 0;
}



static void mipi_mot_set_backlight(struct msm_fb_data_type *mfd)
{
	struct mipi_panel_info *mipi;
	static int bl_level_old;

	mipi  = &mfd->panel_info.mipi;
	if (bl_level_old == mfd->bl_level)
		return;

	mutex_lock(&mfd->dma->ov_mutex);
	/* mdp4_dsi_cmd_busy_wait: will turn on dsi clock also */
	mdp4_dsi_cmd_dma_busy_wait(mfd);
	mdp4_dsi_blt_dmap_busy_wait(mfd);

	led_pwm1[1] = (unsigned char)(mfd->bl_level);
	mipi_dsi_cmds_tx(mfd, &mot_tx_buf, mot_cmd_backlight_cmds,
			ARRAY_SIZE(mot_cmd_backlight_cmds));
	bl_level_old = mfd->bl_level;
	mutex_unlock(&mfd->dma->ov_mutex);
	return;
}

static int __devinit mipi_mot_lcd_probe(struct platform_device *pdev)
{
	if (pdev->id == 0)
		mipi_mot_pdata = pdev->dev.platform_data;

	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_mot_lcd_probe,
	.driver = {
		.name   = "mipi_mot",
	},
};

static struct msm_fb_panel_data mot_panel_data = {
	.on		= mipi_mot_lcd_on,
	.off		= mipi_mot_lcd_off,
	.set_backlight = mipi_mot_set_backlight,
};


static int ch_used[3];

int mipi_mot_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_mot", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	mot_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &mot_panel_data,
		sizeof(mot_panel_data));
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

static int __init mipi_mot_lcd_init(void)
{
	mipi_dsi_buf_alloc(&mot_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&mot_rx_buf, DSI_BUF_SIZE);

	return platform_driver_register(&this_driver);
}

module_init(mipi_mot_lcd_init);
