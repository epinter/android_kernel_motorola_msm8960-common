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
#include "mdp4.h"

static struct mipi_dsi_panel_platform_data *mipi_mot_pdata;

static struct mipi_mot_panel mot_panel;

static struct dsi_buf mot_tx_buf;
static struct dsi_buf mot_rx_buf;


static u16 get_manufacture_id(struct msm_fb_data_type *mfd)
{
	static u16 manufacture_id = INVALID_VALUE;

	if (manufacture_id == INVALID_VALUE) {
		if (mot_panel.get_manufacture_id)
			manufacture_id = mot_panel.get_manufacture_id(mfd);
		else {
			pr_err("%s: can not locate get_manufacture_id()\n",
								__func__);
			goto end;
		}

		pr_info(" MIPI panel Manufacture_id = 0x%x\n", manufacture_id);
	}

end:
	return manufacture_id;
}

static u16 get_controller_ver(struct msm_fb_data_type *mfd)
{
	static u16 controller_ver = INVALID_VALUE;

	if (controller_ver == INVALID_VALUE) {
		if (mot_panel.get_controller_ver)
			controller_ver = mot_panel.get_controller_ver(mfd);
		else {
			pr_err("%s: can not locate get_controller_ver()\n",
								__func__);
			goto end;
		}

		pr_info(" MIPI panel Controller_ver = 0x%x\n", controller_ver);
	}
end:
	return controller_ver;
}


static u16 get_controller_drv_ver(struct msm_fb_data_type *mfd)
{
	static u16 controller_drv_ver = INVALID_VALUE;

	if (controller_drv_ver == INVALID_VALUE) {
		if (mot_panel.get_controller_drv_ver)
			controller_drv_ver =
				mot_panel.get_controller_drv_ver(mfd);
		else {
			pr_err("%s: cannot locate get_controller_drv_ver()\n",
								__func__);
			goto end;
		}

		pr_info(" MIPI panel Controller_drv_ver = 0x%x\n",
							controller_drv_ver);
	}

end:
        return controller_drv_ver;
}

static int mipi_mot_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	int ret = 0;

	pr_info("%s is called\n", __func__);

	mfd = platform_get_drvdata(pdev);


	if (!mfd) {
		pr_err("%s: invalid mfd\n", __func__);
		ret = -ENODEV;
		goto err;
	}

	if (mfd->key != MFD_KEY) {
		pr_err("%s: Invalid key\n", __func__);
		ret = -EINVAL;
		goto err;
	}


	get_manufacture_id(mfd);
	get_controller_ver(mfd);
	get_controller_drv_ver(mfd);

	if (mot_panel.panel_enable)
		mot_panel.panel_enable(mfd);
	else {
		pr_err("%s: no panel support\n", __func__);
		ret = -ENODEV;
		goto err;
	}

	pr_info("%s completed\n", __func__);

	return 0;
err:
	return ret;
}

static int mipi_mot_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	int ret = 0;

	pr_info("%s is called\n", __func__);

	mfd = platform_get_drvdata(pdev);

	if (!mfd) {
		pr_err("%s: invalid mfd\n", __func__);
		ret = -ENODEV;
		goto err;
	}
	if (mfd->key != MFD_KEY) {
		pr_err("%s: Invalid key\n", __func__);
		ret = -EINVAL;
		goto err;
	}

	if (mot_panel.panel_disable)
		mot_panel.panel_disable(mfd);
	else {
		pr_err("%s: no panel support\n", __func__);
		ret = -ENODEV;
		goto err;
	}

	pr_info("%s completed\n", __func__);

	return 0;

err:
	return ret;
}

static int __devinit mipi_mot_lcd_probe(struct platform_device *pdev)
{
	struct platform_device *lcd_dev;

	if (pdev->id == 0)
		mipi_mot_pdata = pdev->dev.platform_data;

	lcd_dev = msm_fb_add_device(pdev);
	if (!lcd_dev)
		pr_err("%s: Failed to add lcd device\n", __func__);

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
};


struct mipi_mot_panel *mipi_mot_get_mot_panel(void)
{
	return &mot_panel;
}

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

	if (mot_panel.set_backlight)
		mot_panel_data.set_backlight = mot_panel.set_backlight;

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

	mot_panel.mot_tx_buf = &mot_tx_buf;
	mot_panel.mot_rx_buf = &mot_rx_buf;

	mot_panel.get_manufacture_id = mipi_mot_get_manufacture_id;
	mot_panel.get_controller_ver = mipi_mot_get_controller_ver;
	mot_panel.get_controller_drv_ver = mipi_mot_get_controller_drv_ver;

	return platform_driver_register(&this_driver);
}

module_init(mipi_mot_lcd_init);
