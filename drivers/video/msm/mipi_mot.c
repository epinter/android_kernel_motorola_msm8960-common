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

unsigned short display_hw_rev_txt_manufacturer;
unsigned short display_hw_rev_txt_controller;
unsigned short display_hw_rev_txt_controller_drv;

static u16 get_manufacture_id(struct msm_fb_data_type *mfd)
{
	static u16 manufacture_id = INVALID_VALUE;
	display_hw_rev_txt_manufacturer = 0;

	if (manufacture_id == INVALID_VALUE) {
		if (mot_panel.get_manufacture_id)
			manufacture_id = mot_panel.get_manufacture_id(mfd);
		else {
			pr_err("%s: can not locate get_manufacture_id()\n",
								__func__);
			goto end;
		}

		pr_info(" MIPI panel Manufacture_id = 0x%x\n", manufacture_id);
		display_hw_rev_txt_manufacturer = manufacture_id;
	}

end:
	return manufacture_id;
}

static u16 get_controller_ver(struct msm_fb_data_type *mfd)
{
	static u16 controller_ver = INVALID_VALUE;
	display_hw_rev_txt_controller = 0;

	if (controller_ver == INVALID_VALUE) {
		if (mot_panel.get_controller_ver)
			controller_ver = mot_panel.get_controller_ver(mfd);
		else {
			pr_err("%s: can not locate get_controller_ver()\n",
								__func__);
			goto end;
		}

		pr_info(" MIPI panel Controller_ver = 0x%x\n", controller_ver);
		display_hw_rev_txt_controller = controller_ver;
	}
end:
	return controller_ver;
}


static u16 get_controller_drv_ver(struct msm_fb_data_type *mfd)
{
	static u16 controller_drv_ver = INVALID_VALUE;
	display_hw_rev_txt_controller_drv = 0;

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
		display_hw_rev_txt_controller_drv = controller_drv_ver;
	}

end:
        return controller_drv_ver;
}

static ssize_t panel_acl_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	u8 data = 0xff;

	if (mot_panel.acl_support_present == FALSE) {
		pr_err("%s: panel doesn't support ACL\n", __func__);
		data = -EPERM;
		goto err;
	}

	mutex_lock(&mot_panel.lock);
	if (mot_panel.acl_enabled)
		data = 1;
	else
		data = 0;
	mutex_unlock(&mot_panel.lock);
err:
	return sprintf(buf, "%d\n", ((u32) data));
}

static ssize_t panel_acl_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long acl_val = 0;
	unsigned long r = 0;

	if (mot_panel.mfd == 0) {
		r = -ENODEV;
		goto end;
	}
	if (mot_panel.acl_support_present == TRUE) {
		r = strict_strtoul(buf, 0, &acl_val);
		if ((r) || ((acl_val != 0) && (acl_val != 1))) {
			pr_err("%s: Invalid ACL value = %lu\n",
							__func__, acl_val);
			r = -EINVAL;
			goto end;
		}
		mutex_lock(&mot_panel.lock);
		if (mot_panel.acl_enabled != acl_val) {
			mot_panel.acl_enabled = acl_val;
			mot_panel.enable_acl(mot_panel.mfd);
		}
		mutex_unlock(&mot_panel.lock);
	}

end:
	return r ? r : count;
}
static DEVICE_ATTR(acl_mode, S_IRUGO | S_IWGRP,
					panel_acl_show, panel_acl_store);
static struct attribute *acl_attrs[] = {
	&dev_attr_acl_mode.attr,
	NULL,
};
static struct attribute_group acl_attr_group = {
	.attrs = acl_attrs,
};

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
	int ret = 0;

	if (pdev->id == 0)
		mipi_mot_pdata = pdev->dev.platform_data;

	lcd_dev = msm_fb_add_device(pdev);
	if (!lcd_dev)
		pr_err("%s: Failed to add lcd device\n", __func__);

	mutex_init(&mot_panel.lock);
	if (mot_panel.acl_support_present == TRUE) {
		mot_panel.mfd = platform_get_drvdata(lcd_dev);
		if (!mot_panel.mfd) {
			pr_err("%s: invalid mfd\n", __func__);
			ret = -ENODEV;
			goto err;
		}
		ret = sysfs_create_group(&mot_panel.mfd->fbi->dev->kobj,
                                                       &acl_attr_group);
		if (ret < 0) {
			pr_err("%s: acl_mode file creation failed\n", __func__);
			goto err;
		}
		/* Set the default ACL value to the LCD */
		mot_panel.enable_acl(mot_panel.mfd);
	}
	return 0;
err:
	return ret;
}

static int __devexit mipi_mot_lcd_remove(struct platform_device *pdev)
{
	if (mot_panel.acl_support_present == TRUE) {
		sysfs_remove_group(&mot_panel.mfd->fbi->dev->kobj,
							&acl_attr_group);
	}
	mutex_destroy(&mot_panel.lock);
	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_mot_lcd_probe,
	.remove = mipi_mot_lcd_remove,
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
