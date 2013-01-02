/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 * Copyright (c) 2011, Motorola Mobility, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef MIPI_MOT_PANEL_H
#define MIPI_MOT_PANEL_H

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

#define INVALID_VALUE   0xFFF

struct mipi_mot_panel {

	struct msm_panel_info pinfo;

	struct dsi_buf *mot_tx_buf;
	struct dsi_buf *mot_rx_buf;

	int (*panel_enable) (struct msm_fb_data_type *mfd);
	int (*panel_disable) (struct msm_fb_data_type *mfd);
	int (*panel_on)(struct msm_fb_data_type *mfd);
	int (*panel_off)(struct msm_fb_data_type *mfd);
	int (*esd_run) (struct msm_fb_data_type *mfd);
	void (*set_backlight)(struct msm_fb_data_type *mfd);
	u16 (*get_manufacture_id)(struct msm_fb_data_type *mfd);
	u16 (*get_controller_ver)(struct msm_fb_data_type *mfd);
	u16 (*get_controller_drv_ver)(struct msm_fb_data_type *mfd);
};

int mipi_mot_device_register(struct msm_panel_info *pinfo, u32 channel,
								u32 panel);

struct mipi_mot_panel *mipi_mot_get_mot_panel(void);


u16 mipi_mot_get_manufacture_id(struct msm_fb_data_type *mfd);
u16 mipi_mot_get_controller_ver(struct msm_fb_data_type *mfd);
u16 mipi_mot_get_controller_drv_ver(struct msm_fb_data_type *mfd);

#endif /* MIPI_MOT_PANEL_H */
