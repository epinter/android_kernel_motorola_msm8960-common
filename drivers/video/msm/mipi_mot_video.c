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
#include "mipi_mot_video.h"
#include "mdp4.h"

static struct mipi_dsi_panel_platform_data *mipi_mot_pdata;

static struct dsi_buf mot_tx_buf;
static struct dsi_buf mot_rx_buf;

static char enter_sleep[2] = {0x10, 0x00}; /* DTYPE_DCS_WRITE */
static char exit_sleep[2] = {0x11, 0x00}; /* DTYPE_DCS_WRITE */

static char display_off[2] = {0x28, 0x00}; /* DTYPE_DCS_WRITE */
static char display_on[2] = {0x29, 0x00}; /* DTYPE_DCS_WRITE */

static char MTP_key_enable_1[3] = {0xf0, 0x5a, 0x5a}; /* DTYPE_DCS_WRITE */
static char MTP_key_enable_2[3] = {0xf1, 0x5a, 0x5a}; /* DTYPE_DCS_WRITE */

static char  panel_condition_set[39] = {
	0xf8,
	0x01,
	0x30,
	0x00,
	0x00,
	0x00,
	0x97,
	0x00,
	0x41,
	0x77,
	0x10,
	0x28,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x20,
	0x02,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x02,
	0x08,
	0x08,
	0x20,
	0x20,
	0xc0,
	0xc1,
	0x01,
	0x81,
	0xc1,
	0x00,
	0xc8,
	0xc1,
	0xd3,
	0xd1}; /* DTYPE_DCS_LWRITE */

static char display_condition_set[4] = {0xf2, 0x80, 0x03, 0x0d}; /* DTYPE_DCS_LWRITE */

static char gamma_settings[26] = {
	0xfa,
	0x01,
	0x61,
	0x46,
	0x6a,
	0xe4,
	0xef,
	0xd7,
	0xca,
	0xbc,
	0xd2,
	0xce,
	0xc7,
	0xcd,
	0x9e,
	0x95,
	0x96,
	0xb6,
	0xaa,
	0xb0,
	0x00,
	0xd4,
	0x00,
	0xbe,
	0x00,
	0xf6}; /* DTYPE_DCS_LWRITE */

static char gamma_ltps_set_update[2] = {0xf7, 0x03}; /* DTYPE_DCS_WRITE1 */

static char etc_condition_set_source_control[4] = {0xf6, 0x00, 0x02, 0x00}; /* DTYPE_DCS_LWRITE */

static char etc_condition_set_pentile_control[10] = {
	0xb6,
	0x0c,
	0x02,
	0x03,
	0x32,
	0xff,
	0x44,
	0x44,
	0xc0,
	0x00}; /* DTYPE_DCS_LWRITE */

static char etc_condition_set_nvm_setting[15] = {
	0xd9,
	0x14,
	0x40,
	0x0c,
	0xcb,
	0xce,
	0x6e,
	0xc4,
	0x07,
	0x40,
	0x41,
	0xd0,
	0x00,
	0x60,
	0x19}; /* DTYPE_DCS_LWRITE */

static char etc_condition_set_power_control[8] = {
	0xf4,
	0xcf,
	0x0a,
	0x12,
	0x10,
	0x1e,
	0x33,
	0x02}; /* DTYPE_DCS_LWRITE */

#define DEFAULT_DELAY 1

static struct dsi_cmd_desc mot_video_on_cmds1[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, DEFAULT_DELAY,
		sizeof(MTP_key_enable_1), MTP_key_enable_1},
	{DTYPE_DCS_WRITE, 1, 0, 0, DEFAULT_DELAY,
		sizeof(MTP_key_enable_2), MTP_key_enable_2},
	{DTYPE_DCS_WRITE, 1, 0, 0, DEFAULT_DELAY,
		sizeof(exit_sleep), exit_sleep},
};

static struct dsi_cmd_desc mot_video_on_cmds2[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, DEFAULT_DELAY,
		sizeof(panel_condition_set), panel_condition_set},
	{DTYPE_DCS_LWRITE, 1, 0, 0, DEFAULT_DELAY,
		sizeof(display_condition_set), display_condition_set},
	{DTYPE_DCS_LWRITE, 1, 0, 0, DEFAULT_DELAY,
		sizeof(gamma_settings), gamma_settings},
	{DTYPE_DCS_WRITE1, 1, 0, 0, DEFAULT_DELAY,
		sizeof(gamma_ltps_set_update), gamma_ltps_set_update},
	{DTYPE_DCS_LWRITE, 1, 0, 0, DEFAULT_DELAY,
		sizeof(etc_condition_set_source_control), etc_condition_set_source_control},
	{DTYPE_DCS_LWRITE, 1, 0, 0, DEFAULT_DELAY,
		sizeof(etc_condition_set_pentile_control), etc_condition_set_pentile_control},
	{DTYPE_DCS_LWRITE, 1, 0, 0, DEFAULT_DELAY,
		sizeof(etc_condition_set_nvm_setting), etc_condition_set_nvm_setting},
	{DTYPE_DCS_LWRITE, 1, 0, 0, DEFAULT_DELAY,
		sizeof(etc_condition_set_power_control), etc_condition_set_power_control},
};

static struct dsi_cmd_desc mot_video_on_cmds3[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, DEFAULT_DELAY,
		sizeof(display_on), display_on},
};


static struct dsi_cmd_desc mot_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(enter_sleep), enter_sleep}
};

static char manufacture_id[2] = {0xda}; /* DTYPE_DCS_READ */

static struct dsi_cmd_desc mot_manufacture_id_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(manufacture_id), manufacture_id};

static uint32 mipi_mot_manufacture_id(struct msm_fb_data_type *mfd)
{
	struct dsi_buf *rp, *tp;
	struct dsi_cmd_desc *cmd;
	uint32 *lp;

	tp = &mot_tx_buf;
	rp = &mot_rx_buf;
	mipi_dsi_buf_init(rp);
	mipi_dsi_buf_init(tp);

	cmd = &mot_manufacture_id_cmd;
	mipi_dsi_cmds_rx(mfd, tp, rp, cmd, 1);

	lp = (uint32 *)rp->data;
	pr_info("%s: manufacture_id[%x]=%2x %2x %2x %2x (total %d)", __func__, (unsigned int)manufacture_id[0], (unsigned int)rp->data[0], (unsigned int)rp->data[1], (unsigned int)rp->data[2], (unsigned int)rp->data[3], (int)rp->len);
	return *lp;
}

static uint32 mipi_mot_manufacture_id_ex(struct msm_fb_data_type *mfd, char reg)
{
	manufacture_id[0] = reg;
	return mipi_mot_manufacture_id(mfd);
}

static int mipi_mot_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	struct msm_panel_info *pinfo;

	pr_info("%s\n", __func__);

	mfd = platform_get_drvdata(pdev);
	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	pinfo = &mfd->panel_info;
	mipi  = &mfd->panel_info.mipi;

	mipi_dsi_cmds_tx(mfd, &mot_tx_buf, mot_video_on_cmds1, ARRAY_SIZE(mot_video_on_cmds1));

	msleep(5);

	mipi_dsi_cmds_tx(mfd, &mot_tx_buf, mot_video_on_cmds2, ARRAY_SIZE(mot_video_on_cmds2));
	msleep(120);

	mipi_mot_manufacture_id_ex(mfd, 0xda);

	mipi_dsi_cmds_tx(mfd, &mot_tx_buf, mot_video_on_cmds3, ARRAY_SIZE(mot_video_on_cmds3));

	msleep(5);

	pr_info("%s completed\n", __func__);

	return 0;
}

static int mipi_mot_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	pr_info("%s\n", __func__);

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi_dsi_cmds_tx(mfd, &mot_tx_buf,mot_display_off_cmds,
			ARRAY_SIZE(mot_display_off_cmds));

	pr_info("%s completed\n", __func__);

	return 0;
}

static void mipi_mot_set_backlight(struct msm_fb_data_type *mfd)
{
	pr_info("%s\n", __func__);

#if 0
	struct mipi_panel_info *mipi;
	static int bl_level_old;
	mipi  = &mfd->panel_info.mipi;

	if (bl_level_old == mfd->bl_level)
		return;

	mutex_lock(&mfd->dma->ov_mutex);
	mdp4_dsi_cmd_dma_busy_wait(mfd);
	mdp4_dsi_blt_dmap_busy_wait(mfd);

	mipi_dsi_cmds_tx(mfd, &mot_tx_buf, mot_cmd_backlight_cmds,
		ARRAY_SIZE(mot_cmd_backlight_cmds));
	bl_level_old = mfd->bl_level;
	mutex_unlock(&mfd->dma->ov_mutex);
#endif

	return;
}

static int __devinit mipi_mot_lcd_probe(struct platform_device *pdev)
{
	void *ret;

	if (pdev->id == 0) {
		mipi_mot_pdata = pdev->dev.platform_data;
		return 0;
	}

	ret = msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_mot_lcd_probe,
	.driver = {
		.name   = "mipi_mot_video",
	},
};

static struct msm_fb_panel_data mot_panel_data = {
	.on		= mipi_mot_lcd_on,
	.off		= mipi_mot_lcd_off,
	.set_backlight = mipi_mot_set_backlight,
};

static int ch_used[3];

int mipi_mot_video_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_mot_video", (panel << 8)|channel);
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
