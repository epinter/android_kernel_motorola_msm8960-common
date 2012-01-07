/* drivers/input/touchscreen/melfas100_ts.c
 *
 * Copyright (C) 2011 Motorola Mobility, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Based on Melfas Touchscreen driver for MMS-100 series.
 *
 */


#include <linux/module.h>
#include <linux/delay.h>
#include <linux/earlysuspend.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>

#include <linux/melfas100_ts.h>

#define TS_MAX_Z_TOUCH	255
#define TS_MAX_W_TOUCH	100
#define TS_MAX_X_COORD	700
#define TS_MAX_Y_COORD	1260

/* Register Map */
#define TS_MODE_CONTROL		0x01
#define TS_XY_RES_HIGH		0x02
#define TS_X_RES_LOW		0x03
#define TS_Y_RES_LOW		0x04
#define TS_CONTACT_THRES	0x05
#define TS_MOVING_THRES		0x06
#define TS_INPUT_EVENT_PKT_SZ	0x0F
#define TS_INPUT_EVENT_INFO	0x10
#define TS_VERSION_INFO		0xF0

#define TS_READ_REGS_LEN	66
#define MELFAS_MAX_TOUCH	10

#define TS_DBG_LVL_0	0
#define TS_DBG_LVL_1	1
#define TS_DBG_LVL_2	2
#define TS_DBG_LVL_3	3

struct muti_touch_info {
	int xpos;
	int ypos;
	int width;
	int strength;
};

static uint8_t tsdebug;
static struct muti_touch_info mtouch_info[MELFAS_MAX_TOUCH];

struct melfas_ts_version_info {
	uint8_t panel_ver;
	uint8_t hw_ver;
	uint8_t hw_comp_grp;
	uint8_t core_fw_ver;
	uint8_t priv_fw_ver;
	uint8_t pub_fw_ver;
};

struct melfas_ts_data {
	struct i2c_client *client;
	struct device *dev;
	struct input_dev *input_dev;
	struct melfas_ts_platform_data *pdata;
	struct early_suspend early_suspend;
	struct melfas_ts_version_info version_info;
	atomic_t irq_enabled;
};


#ifdef CONFIG_HAS_EARLYSUSPEND
static void melfas_ts_early_suspend(struct early_suspend *h);
static void melfas_ts_late_resume(struct early_suspend *h);
#endif

static void melfas_ts_get_data(struct melfas_ts_data *ts)
{
	int ret = 0, i;
	uint8_t buf[TS_READ_REGS_LEN];
	int read_num, finger_id;

	ret = i2c_smbus_read_i2c_block_data(ts->client,
			 TS_INPUT_EVENT_PKT_SZ, 1, (u8 *)buf);
	if (ret < 0) {
		pr_err("%s: i2c failed\n", __func__);
		return ;
	}

	read_num = buf[0];

	if (read_num > 0) {

		ret = i2c_smbus_read_i2c_block_data(ts->client,
			 TS_INPUT_EVENT_INFO, read_num, (u8 *)buf);
		if (ret < 0) {
			pr_err("%s: i2c failed\n", __func__);
			return ;
		}

		for (i = 0; i < read_num; i = i+6) {
			finger_id = (buf[i] & 0x0F)-1;

			mtouch_info[finger_id].xpos =
				(uint16_t)(buf[i+1] & 0x0F) << 8 | buf[i+2];
			mtouch_info[finger_id].ypos =
				(uint16_t)(buf[i+1] &	0xF0) << 4 | buf[i+3];

			if (ts->pdata->flags & TS_FLIP_X)
				mtouch_info[finger_id].xpos =
				 TS_MAX_X_COORD - mtouch_info[finger_id].xpos;

			if (ts->pdata->flags & TS_FLIP_Y)
				mtouch_info[finger_id].ypos =
				 TS_MAX_Y_COORD - mtouch_info[finger_id].ypos;

			if ((buf[i] & 0x80) == 0)
				mtouch_info[finger_id].strength = 0;
			else
				mtouch_info[finger_id].strength = buf[i+4];

			mtouch_info[finger_id].width = buf[i+5];
		}

		for (i = 0; i < MELFAS_MAX_TOUCH; i++) {
			if (mtouch_info[i].strength == -1)
				continue;

			input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, i);
			input_report_abs(ts->input_dev, ABS_MT_POSITION_X,
					 mtouch_info[i].xpos);
			input_report_abs(ts->input_dev, ABS_MT_POSITION_Y,
					 mtouch_info[i].ypos);
			input_report_abs(ts->input_dev, ABS_MT_PRESSURE,
					 mtouch_info[i].strength);
			input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR,
					 mtouch_info[i].width);
			input_mt_sync(ts->input_dev);

			if (tsdebug >= TS_DBG_LVL_1)
				pr_info("Touch ID: %d, State : %d, x: %d, "
					"y: %d, z: %d w: %d\n",
					 i, (mtouch_info[i].strength > 0),
					mtouch_info[i].xpos,
					 mtouch_info[i].ypos,
					mtouch_info[i].strength,
					mtouch_info[i].width);

			if (mtouch_info[i].strength == 0)
				mtouch_info[i].strength = -1;
		}

		input_sync(ts->input_dev);
	}
}

static irqreturn_t melfas_ts_irq_handler(int irq, void *handle)
{
	struct melfas_ts_data *ts = (struct melfas_ts_data *)handle;

	if (tsdebug >= TS_DBG_LVL_3)
		pr_info("%s:melfas_ts_irq_handler\n", __func__);

	melfas_ts_get_data(ts);
	return IRQ_HANDLED;
}


static int melfas_ts_reset(struct melfas_ts_data *ts)
{
	gpio_set_value(ts->pdata->gpio_vdd_en, 0);
	msleep(50);
	gpio_set_value(ts->pdata->gpio_vdd_en, 1);
	msleep(150);
	return 0;
}

static void hw_reboot(struct melfas_ts_data *ts, bool bootloader)
{
	gpio_direction_output(ts->pdata->gpio_vdd_en, 0);
	gpio_direction_output(ts->pdata->gpio_sda, bootloader ? 0 : 1);
	gpio_direction_output(ts->pdata->gpio_scl, bootloader ? 0 : 1);
	gpio_direction_output(ts->pdata->gpio_resetb, 0);
	msleep(30);
	gpio_set_value(ts->pdata->gpio_vdd_en, 1);
	msleep(30);

	if (bootloader) {
		gpio_set_value(ts->pdata->gpio_scl, 0);
		gpio_set_value(ts->pdata->gpio_sda, 1);
	} else {
		gpio_set_value(ts->pdata->gpio_resetb, 1);
		gpio_direction_input(ts->pdata->gpio_resetb);
		gpio_direction_input(ts->pdata->gpio_scl);
		gpio_direction_input(ts->pdata->gpio_sda);
	}
	msleep(40);
}

/* Driver debugging */
static ssize_t melfas_ts_drv_debug_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "Debug Setting: %u\n", tsdebug);
}

static ssize_t melfas_ts_drv_debug_store(struct device *dev,
		 struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned long value;
	int err = kstrtoul(buf, 0, &value);
	if (err < 0) {
		pr_err("%s: illegal sysfs data\n", __func__);
		return -EINVAL;
	}

	switch (value) {
	case TS_DBG_LVL_0:
	case TS_DBG_LVL_1:
	case TS_DBG_LVL_2:
	case TS_DBG_LVL_3:
		pr_info("%s: Debug setting=%d\n", __func__, (int)value);
		tsdebug = value;
		break;
	default:
		pr_err("%s: Invalid option, 1/0 = "
		       "Intr enable/disable setting=%d\n",
			__func__, (int)value);
		break;
	}

	return size;
}
static DEVICE_ATTR(drv_debug, S_IWUSR | S_IRUGO, melfas_ts_drv_debug_show,
		   melfas_ts_drv_debug_store);

/* interrupt status */
static ssize_t melfas_ts_hw_irqstat_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int err = 0;
	struct melfas_ts_data *ts = dev_get_drvdata(dev);

	err = gpio_get_value(ts->pdata->gpio_resetb);
	switch (err) {
	case 0:
		err = sprintf(buf, "Interrupt line is LOW.\n");
		break;
	case 1:
		err = sprintf(buf, "Interrupt line is HIGH.\n");
		break;
	default:
		err = sprintf(buf, "Function irq_stat() returned %d.\n", err);
		break;
	}

	return err;
}
static DEVICE_ATTR(hw_irqstat, S_IWUSR | S_IRUGO,
		   melfas_ts_hw_irqstat_show, NULL);

/* Interrupt enable/disable */
static ssize_t melfas_ts_irq_enabled_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct melfas_ts_data *ts = dev_get_drvdata(dev);
	return sprintf(buf, "Interrupt: %u\n",
		       atomic_read(&ts->irq_enabled));
}

static ssize_t melfas_ts_irq_enabled_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct melfas_ts_data *ts = dev_get_drvdata(dev);
	unsigned long value;
	int err = kstrtoul(buf, 0, &value);
	if (err < 0) {
		pr_err("%s: illegal sysfs data\n", __func__);
		return -EINVAL;
	}

	switch (value) {
	case 0:
		if (atomic_cmpxchg(&ts->irq_enabled, 1, 0)) {
			pr_info("%s: touch irq disabled, %d\n",
				__func__, atomic_read(&ts->irq_enabled));
			disable_irq_nosync(ts->client->irq);
		}
		break;
	case 1:
		if (!atomic_cmpxchg(&ts->irq_enabled, 0, 1)) {
			pr_info("%s: touch irq enabled, %d\n",
				__func__, atomic_read(&ts->irq_enabled));
			enable_irq(ts->client->irq);
		}
		break;
	default:
		pr_info("melfas_irq_enable failed -> irq_enabled = %d\n",
			atomic_read(&ts->irq_enabled));
		break;
	}

	return size;
}
static DEVICE_ATTR(irq_enabled, S_IWUSR | S_IRUGO, melfas_ts_irq_enabled_show,
		   melfas_ts_irq_enabled_store);

static ssize_t melfas_ts_hw_recov_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct melfas_ts_data *ts = dev_get_drvdata(dev);
	struct i2c_adapter *adapter = to_i2c_adapter(ts->client->dev.parent);
	unsigned long value;
	int err = kstrtoul(buf, 0, &value);
	if (err < 0) {
		pr_err("%s: illegal sysfs data\n", __func__);
		return -EINVAL;
	}

	if (value) {
		pr_info("%s: Setting IC in bootloader mode\n", __func__);
		disable_irq_nosync(ts->client->irq);
		i2c_lock_adapter(adapter);
		ts->pdata->mux_fw_flash(true);
		hw_reboot(ts, true);
		pr_info("%s: IC is in bootloader mode\n", __func__);

	} else {
		pr_info("%s: Setting IC in Normal mode\n", __func__);
		hw_reboot(ts, false);
		ts->pdata->mux_fw_flash(false);
		melfas_ts_reset(ts);
		i2c_unlock_adapter(adapter);
		enable_irq(ts->client->irq);
		pr_info("%s: IC is in Normal mode\n", __func__);
	}

	return size;
}
static DEVICE_ATTR(hw_recov, S_IWUSR | S_IRUGO, NULL,
		   melfas_ts_hw_recov_store);

static ssize_t melfas_ts_hw_reset_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct melfas_ts_data *ts = dev_get_drvdata(dev);
	unsigned long value;
	int err = kstrtoul(buf, 0, &value);
	if (err < 0) {
		pr_err("%s: illegal sysfs data\n", __func__);
		return -EINVAL;
	}

	disable_irq_nosync(ts->client->irq);

	if (value) {
		melfas_ts_reset(ts);
		pr_info("%s: IC has been reset\n", __func__);
	}

	enable_irq(ts->client->irq);

	return size;
}
static DEVICE_ATTR(hw_reset, S_IWUSR | S_IRUGO, NULL,
		   melfas_ts_hw_reset_store);

static ssize_t melfas_ts_ic_ver_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct melfas_ts_data *ts = dev_get_drvdata(dev);

	return sprintf(buf, " Panel Version: %x, HW Revision: %x\n"
		       "HW Comp Grp: %x,"
		       " Core FW ver: %x\n Private FW ver: %x,"
		       "Public FW ver: %x\n",\
		 ts->version_info.panel_ver, ts->version_info.hw_ver, \
		 ts->version_info.hw_comp_grp, ts->version_info.core_fw_ver, \
		 ts->version_info.priv_fw_ver, ts->version_info.pub_fw_ver);
}
static DEVICE_ATTR(ic_ver, S_IWUSR | S_IRUGO, melfas_ts_ic_ver_show, NULL);

static int melfas_ts_probe(struct i2c_client *client,
				 const struct i2c_device_id *id)
{
	struct melfas_ts_platform_data *pdata = client->dev.platform_data;
	struct melfas_ts_data *ts;
	int ret = 0, i;

	if (tsdebug >= TS_DBG_LVL_3)
		pr_info("%s:melfas_ts probe\n", __func__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("%s: need I2C_FUNC_I2C\n", __func__);
		ret = -ENODEV;
		goto err_check_functionality_failed;
	}

	ts = kzalloc(sizeof(struct melfas_ts_data), GFP_KERNEL);
	if (ts == NULL)	{
		pr_err("%s: failed to create a state of melfas-ts\n", __func__);
		ret = -ENOMEM;
		goto err_alloc_data_failed;
	}

	ts->pdata = pdata;
	ts->client = client;
	i2c_set_clientdata(client, ts);

	ret = i2c_smbus_read_i2c_block_data(ts->client,
			 TS_VERSION_INFO,
			 sizeof(ts->version_info),
			 (u8 *)&ts->version_info);
	if (ret < 0) {
		pr_err("%s: i2c failed\n", __func__);
		goto err_alloc_data_failed;
	}

	pr_info("%s:\n Panel Version: %x, HW Revision: %x\n HW Comp Grp: %x,"
		" Core FW ver: %x\n Private FW ver: %x, "
		"Public FW ver: %x\n", __func__, ts->version_info.panel_ver,
		ts->version_info.hw_ver, ts->version_info.hw_comp_grp,
		ts->version_info.core_fw_ver,
		ts->version_info.priv_fw_ver, ts->version_info.pub_fw_ver);


	ts->input_dev = input_allocate_device();
	if (!ts->input_dev) {
		pr_err("%s: Not enough memory\n", __func__);
		ret = -ENOMEM;
		goto err_input_dev_alloc_failed;
	}

	ts->input_dev->name = "melfas-ts" ;
	input_set_drvdata(ts->input_dev, ts);

	set_bit(EV_SYN, ts->input_dev->evbit);
	set_bit(EV_ABS, ts->input_dev->evbit);
	set_bit(INPUT_PROP_DIRECT, ts->input_dev->propbit);
	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_X, 0,
			     TS_MAX_X_COORD, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_Y, 0,
			     TS_MAX_Y_COORD, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_PRESSURE, 0,
			     TS_MAX_Z_TOUCH, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0,
			     TS_MAX_W_TOUCH, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_TRACKING_ID, 0,
			     MELFAS_MAX_TOUCH, 0, 0);

	ret = input_register_device(ts->input_dev);
	if (ret) {
		pr_err("%s: Failed to register device\n", __func__);
		ret = -ENOMEM;
		goto err_input_register_device_failed;
	}

	if (ts->client->irq) {
		ret = request_threaded_irq(client->irq, NULL,
					   melfas_ts_irq_handler,
					   IRQF_TRIGGER_LOW | IRQF_ONESHOT,
					   ts->client->name, ts);
		if (ret > 0) {
			pr_err("%s: Can't allocate irq %d, ret %d\n",
			       __func__, ts->client->irq, ret);
			ret = -EBUSY;
			goto err_request_irq;
		} else
			atomic_set(&ts->irq_enabled, 1);
	}

	ret = device_create_file(&ts->client->dev, &dev_attr_drv_debug);
	if (ret) {
		pr_err("%s:File device creation failed: %d\n", __func__, ret);
		ret = -ENODEV;
		goto err_create_dbg_device_failed;
	}

	ret = device_create_file(&ts->client->dev, &dev_attr_irq_enabled);
	if (ret) {
		pr_err("%s:File device creation failed: %d\n", __func__, ret);
		ret = -ENODEV;
		goto err_create_irq_enabled_device_failed;
	}

	ret = device_create_file(&ts->client->dev, &dev_attr_hw_recov);
	if (ret) {
		pr_err("%s:File device creation failed: %d\n", __func__, ret);
		ret = -ENODEV;
		goto err_create_hw_recov_device_failed;
	}


	ret = device_create_file(&ts->client->dev, &dev_attr_hw_reset);
	if (ret) {
		pr_err("%s:File device creation failed: %d\n", __func__, ret);
		ret = -ENODEV;
		goto err_create_hw_reset_device_failed;
	}

	ret = device_create_file(&ts->client->dev, &dev_attr_ic_ver);
	if (ret) {
		pr_err("%s:File device creation failed: %d\n", __func__, ret);
		ret = -ENODEV;
		goto err_create_ic_ver_device_failed;
	}

	ret = device_create_file(&ts->client->dev, &dev_attr_hw_irqstat);
	if (ret) {
		pr_err("%s:irqstat device creation failed: %d\n",
		       __func__, ret);
		ret = -ENODEV;
	}

	for (i = 0; i < MELFAS_MAX_TOUCH ; i++)  /* _SUPPORT_MULTITOUCH_ */
		mtouch_info[i].strength = -1;


#if CONFIG_HAS_EARLYSUSPEND
	ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	ts->early_suspend.suspend = melfas_ts_early_suspend;
	ts->early_suspend.resume = melfas_ts_late_resume;
	register_early_suspend(&ts->early_suspend);
#endif

	if (tsdebug >= TS_DBG_LVL_1)
		pr_info("%s: Melfas Touchscreen: %s, irq: %d\n",
			__func__, ts->client->name, ts->client->irq);

	return 0;

err_create_ic_ver_device_failed:
	pr_err("%s: device creation for ic_ver failed\n", __func__);
	device_remove_file(ts->dev, &dev_attr_hw_reset);
err_create_hw_reset_device_failed:
	pr_err("%s: device creation for hw_reset failed\n", __func__);
	device_remove_file(ts->dev, &dev_attr_hw_recov);
err_create_hw_recov_device_failed:
	pr_err("%s: device creation for hw_recov failed\n", __func__);
	device_remove_file(ts->dev, &dev_attr_irq_enabled);
err_create_irq_enabled_device_failed:
	pr_err("%s: device creation for irq_enabled failed\n", __func__);
	device_remove_file(ts->dev, &dev_attr_drv_debug);
err_create_dbg_device_failed:
	pr_err("%s: device creation for dbg failed\n", __func__);
	input_unregister_device(ts->input_dev);
err_request_irq:
	pr_err("%s: err_request_irq failed\n", __func__);
	free_irq(client->irq, ts);
err_input_register_device_failed:
	pr_err("%s: err_input_register_device failed\n", __func__);
	input_free_device(ts->input_dev);
err_input_dev_alloc_failed:
	pr_err("%s: err_input_dev_alloc failed\n", __func__);
err_alloc_data_failed:
	pr_err("%s: err_alloc_data failed_\n", __func__);
	kfree(ts);
err_check_functionality_failed:
	pr_err("%s: err_check_functionality failed_\n", __func__);

	return ret;
}

static int melfas_ts_remove(struct i2c_client *client)
{
	struct melfas_ts_data *ts = i2c_get_clientdata(client);

	unregister_early_suspend(&ts->early_suspend);
	free_irq(client->irq, ts);
	input_unregister_device(ts->input_dev);
	device_remove_file(ts->dev, &dev_attr_hw_irqstat);
	device_remove_file(ts->dev, &dev_attr_ic_ver);
	device_remove_file(ts->dev, &dev_attr_hw_reset);
	device_remove_file(ts->dev, &dev_attr_hw_recov);
	device_remove_file(ts->dev, &dev_attr_drv_debug);
	device_remove_file(ts->dev, &dev_attr_irq_enabled);
	i2c_set_clientdata(client, NULL);
	kfree(ts);
	return 0;
}

static int melfas_ts_suspend(struct i2c_client *client, pm_message_t mesg)
{
	struct melfas_ts_data *ts = i2c_get_clientdata(client);
	int ret, i;

	if (!atomic_read(&ts->irq_enabled))
		return 0;

	for (i = 0; i < MELFAS_MAX_TOUCH ; i++)	{
		mtouch_info[i].strength = -1;
		mtouch_info[i].xpos = 0;
		mtouch_info[i].ypos = 0;
		mtouch_info[i].width = 0;
	}

	disable_irq(client->irq);

	/* ts deep sleep */
	ret = i2c_smbus_write_byte_data(client, TS_MODE_CONTROL, 0x00);
	if (ret < 0)
		pr_err("%s: i2c_smbus_write_byte_data failed\n", __func__);

	return 0;
}

static int melfas_ts_resume(struct i2c_client *client)
{
	struct melfas_ts_data *ts = i2c_get_clientdata(client);
	int ret;

	if (!atomic_read(&ts->irq_enabled))
		return 0;

	/* ts wake up */
	ret = i2c_smbus_write_byte_data(client, TS_MODE_CONTROL, 0x02);
	if (ret < 0)
		pr_err("%s: i2c_smbus_write_byte_data failed\n", __func__);

	enable_irq(client->irq);

	return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void melfas_ts_early_suspend(struct early_suspend *handler)
{
	struct melfas_ts_data *ts;
	ts = container_of(handler, struct melfas_ts_data, early_suspend);
	melfas_ts_suspend(ts->client, PMSG_SUSPEND);
}

static void melfas_ts_late_resume(struct early_suspend *handler)
{
	struct melfas_ts_data *ts;
	ts = container_of(handler, struct melfas_ts_data, early_suspend);
	melfas_ts_resume(ts->client);
}
#endif

static const struct i2c_device_id melfas_ts_id[] = {
	{MELFAS_TS_NAME, 0},
	{}
};

static struct i2c_driver melfas_ts_driver = {
	.driver = {
	.name = MELFAS_TS_NAME,
	},
	.probe = melfas_ts_probe,
	.remove = __devexit_p(melfas_ts_remove),
	.id_table = melfas_ts_id,
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend = melfas_ts_suspend,
	.resume = melfas_ts_resume,
#endif
};

static int __devinit melfas_ts_init(void)
{
	return i2c_add_driver(&melfas_ts_driver);
}

static void __exit melfas_ts_exit(void)
{
	i2c_del_driver(&melfas_ts_driver);
	return;
}

module_init(melfas_ts_init);
module_exit(melfas_ts_exit);

MODULE_DESCRIPTION("Melfas Touchscreen Controller");
MODULE_AUTHOR("Motorola Mobility");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
