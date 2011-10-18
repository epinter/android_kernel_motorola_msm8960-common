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

static struct msm_panel_info pinfo;

static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = {
		{0x03, 0x0a, 0x04, 0x00, 0x20},	/* regulator */
		/* timing   */
		{0xad, 0x8b, 0x19, 0x00, 0x93, 0x96, 0x1c,
		0x8d, 0x1c, 0x03, 0x04, 0xa0},
		{0x5f, 0x00, 0x00, 0x10},	/* phy ctrl */
		{0xff, 0x00, 0x06, 0x00},	/* strength */
		/* pll control */
		{0x00, 0x8f, 0x01, 0x1a, 0x00, 0x50, 0x48, 0x63,
		/* 4 MIPI lanes */
		0x41, 0x0f, 0x03,
		0x00, 0x14, 0x03, 0x0, 0x02, 0x00, 0x20, 0x00, 0x01},
};

static int __init mipi_cmd_mot_auo_qhd_450_init(void)
{
	int ret;

	pr_info("%s\n", __func__);

#if defined(CONFIG_FB_MSM_MIPI_MOT_DETECT)
	if (msm_fb_detect_client("mipi_mot_cmd_auo_hd_450"))
		return 0;
#endif

	pinfo.xres = 720;
	pinfo.yres = 1280;
	pinfo.type = MIPI_CMD_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;
	pinfo.lcdc.h_back_porch = 50;
	pinfo.lcdc.h_front_porch = 50;
	pinfo.lcdc.h_pulse_width = 20;
	pinfo.lcdc.v_back_porch = 11;
	pinfo.lcdc.v_front_porch = 10;
	pinfo.lcdc.v_pulse_width = 5;
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 100;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 400000000;
	pinfo.lcd.vsync_enable = FALSE;
	pinfo.lcd.hw_vsync_mode = FALSE;
	pinfo.lcd.refx100 = 6000; /* adjust refx100 to prevent tearing */

	pinfo.mipi.mode = DSI_CMD_MODE;
	pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.data_lane2 = TRUE;
	pinfo.mipi.data_lane3 = TRUE;
	pinfo.mipi.t_clk_post = 0x19;
	pinfo.mipi.t_clk_pre = 0x2e;
	pinfo.mipi.stream = 0;	/* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.te_sel = 0; /* TE from vsycn gpio */
	pinfo.mipi.interleave_max = 1;
	pinfo.mipi.insert_dcs_cmd = TRUE;
	pinfo.mipi.wr_mem_continue = 0x3c;
	pinfo.mipi.wr_mem_start = 0x2c;
	pinfo.mipi.dsi_phy_db = &dsi_cmd_mode_phy_db;
        pinfo.mipi.tx_eot_append = 0x01;
        pinfo.mipi.rx_eot_ignore = 0;

	ret = mipi_mot_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_HD);
	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	pr_info("%s device registered\n", __func__);

	return ret;
}


module_init(mipi_cmd_mot_auo_qhd_450_init);
