/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 * Copyright (c) 2011, Motorola Mobility, Inc.
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

static struct mipi_mot_panel *mot_panel;

static char manufacture_id[2] = {DCS_CMD_READ_DA, 0x00}; /* DTYPE_DCS_READ */
static char controller_ver[2] = {DCS_CMD_READ_DB, 0x00};
static char controller_drv_ver[2] = {DCS_CMD_READ_DC, 0x00};

static struct dsi_cmd_desc mot_manufacture_id_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(manufacture_id), manufacture_id};

static struct dsi_cmd_desc mot_controller_ver_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(controller_ver), controller_ver};

static struct dsi_cmd_desc mot_controller_drv_ver_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(controller_drv_ver),
							controller_drv_ver};
static int get_mot_panel(void)
{
	mot_panel = mipi_mot_get_mot_panel();
	if (mot_panel == NULL) {
		pr_err("%s:get mot_panel failed\n", __func__);
		return -1;
	}

	return 0;

}

static uint32 get_panel_info(struct msm_fb_data_type *mfd,
				struct  mipi_mot_panel *mot_panel,
				struct dsi_cmd_desc *cmd)
{
	struct dsi_buf *rp, *tp;
	uint32 *lp;

	tp = mot_panel->mot_tx_buf;
	rp = mot_panel->mot_rx_buf;
	mipi_dsi_buf_init(rp);
	mipi_dsi_buf_init(tp);

	mipi_dsi_cmds_rx(mfd, tp, rp, cmd, 1);
	lp = (uint32 *)rp->data;

	return *lp;
}


u16 mipi_mot_get_manufacture_id(struct msm_fb_data_type *mfd)
{
	struct dsi_cmd_desc *cmd;
	static u16 manufacture_id = INVALID_VALUE;

	if (manufacture_id == INVALID_VALUE) {
		if (get_mot_panel())
			return -1;

		cmd = &mot_manufacture_id_cmd;
		manufacture_id = get_panel_info(mfd, mot_panel, cmd);
	}

	return manufacture_id;
}


u16 mipi_mot_get_controller_ver(struct msm_fb_data_type *mfd)
{
	struct dsi_cmd_desc *cmd;
	static u16 controller_ver = INVALID_VALUE;

	if (controller_ver == INVALID_VALUE) {
		if (get_mot_panel())
			return -1;

		cmd = &mot_controller_ver_cmd;
		controller_ver =  get_panel_info(mfd, mot_panel, cmd);
	}

	return controller_ver;
}


u16 mipi_mot_get_controller_drv_ver(struct msm_fb_data_type *mfd)
{
	struct dsi_cmd_desc *cmd;
	static u16 controller_drv_ver = INVALID_VALUE;

	if (controller_drv_ver == INVALID_VALUE) {
		if (get_mot_panel())
			return -1;

		cmd = &mot_controller_drv_ver_cmd;
		controller_drv_ver = get_panel_info(mfd, mot_panel, cmd);
	}

	return controller_drv_ver;
}


