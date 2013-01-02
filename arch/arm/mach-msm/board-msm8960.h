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
 */

#ifndef __ARCH_ARM_MACH_MSM_BOARD_MSM8960_H
#define __ARCH_ARM_MACH_MSM_BOARD_MSM8960_H

#include <mach/irqs.h>
#include <mach/msm_spi.h>
#include <mach/rpm-regulator.h>
#include <linux/mfd/pm8xxx/pm8921.h>
#include <linux/spi/spi.h>
#include <mach/board.h>
#include <linux/leds.h>
#include <mach/mdm2.h>

/* Macros assume PMIC GPIOs and MPPs start at 1 */
#define PM8921_GPIO_BASE		NR_GPIO_IRQS
#define PM8921_GPIO_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_GPIO_BASE)
#define PM8921_MPP_BASE			(PM8921_GPIO_BASE + PM8921_NR_GPIOS)
#define PM8921_MPP_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_MPP_BASE)
#define PM8921_IRQ_BASE			(NR_MSM_IRQS + NR_GPIO_IRQS)

extern struct pm8921_regulator_platform_data
	msm_pm8921_regulator_pdata[] __devinitdata;

extern int msm_pm8921_regulator_pdata_len __devinitdata;

#define MDM2AP_ERRFATAL			70
#define AP2MDM_ERRFATAL			95
#define MDM2AP_STATUS			69
#define AP2MDM_STATUS			94
#define AP2MDM_PMIC_RESET_N		80
#define AP2MDM_KPDPWR_N			81

#define GPIO_VREG_ID_EXT_5V		0
#define GPIO_VREG_ID_EXT_L2		1
#define GPIO_VREG_ID_EXT_3P3V		2

#define MDP_VSYNC_GPIO 0

#define HAP_SHIFT_LVL_OE_GPIO	47

#define MDP_VSYNC_ENABLED	true
#define MDP_VSYNC_DISABLED	false

#ifdef CONFIG_FB_MSM_TRIPLE_BUFFER
#define MSM_FB_PRIM_BUF_SIZE (1280 * 720 * 4 * 3) /* 4 bpp x 3 pages */
#else
#define MSM_FB_PRIM_BUF_SIZE (1280 * 720 * 4 * 2) /* 4 bpp x 2 pages */
#endif


#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
#define MSM_FB_EXT_BUF_SIZE	(1920 * 1088 * 2 * 1) /* 2 bpp x 1 page */
#elif defined(CONFIG_FB_MSM_TVOUT)
#define MSM_FB_EXT_BUF_SIZE (720 * 576 * 2 * 2) /* 2 bpp x 2 pages */
#else
#define MSM_FB_EXT_BUF_SIZE	0
#endif

#ifdef CONFIG_FB_MSM_OVERLAY_WRITEBACK
/* width x height x 3 bpp x 2 frame buffer */
#define MSM_FB_WRITEBACK_SIZE (1280 * 720 * 3 * 2)
#define MSM_FB_WRITEBACK_OFFSET  \
		(MSM_FB_PRIM_BUF_SIZE + MSM_FB_EXT_BUF_SIZE)
#else
#define MSM_FB_WRITEBACK_SIZE   0
#define MSM_FB_WRITEBACK_OFFSET 0
#endif

#ifdef CONFIG_FB_MSM_HDMI_AS_PRIMARY
/* 4 bpp x 2 page HDMI case */
#define MSM_FB_SIZE roundup((1920 * 1088 * 4 * 2), 4096)
#else
/* Note: must be multiple of 4096 */
#define MSM_FB_SIZE roundup(MSM_FB_PRIM_BUF_SIZE + MSM_FB_EXT_BUF_SIZE + \
				MSM_FB_WRITEBACK_SIZE, 4096)
#endif

#ifdef CONFIG_I2C

#define MSM_8960_GSBI4_QUP_I2C_BUS_ID 4
#define MSM_8960_GSBI3_QUP_I2C_BUS_ID 3
#define MSM_8960_GSBI10_QUP_I2C_BUS_ID 10

#endif

#define MSM_PMEM_ADSP_SIZE         0x3800000
#define MSM_PMEM_AUDIO_SIZE        0x28B000
#ifdef CONFIG_FB_MSM_HDMI_AS_PRIMARY
#define MSM_PMEM_SIZE 0x4000000 /* 64 Mbytes */
#else
#define MSM_PMEM_SIZE 0x1C00000 /* 28 Mbytes */
#endif
#define MSM_RAM_CONSOLE_SIZE       128 * SZ_1K

#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
#define MSM_PMEM_KERNEL_EBI1_SIZE  0xB0C000
#define MSM_ION_EBI_SIZE	(MSM_PMEM_SIZE + 0x600000)
#define MSM_ION_ADSP_SIZE	MSM_PMEM_ADSP_SIZE
#define MSM_ION_HEAP_NUM	4
#else
#define MSM_PMEM_KERNEL_EBI1_SIZE  0x110C000
#define MSM_ION_HEAP_NUM	2
#endif


struct pm8xxx_gpio_init {
	unsigned			gpio;
	struct pm_gpio			config;
};

struct pm8xxx_mpp_init {
	unsigned			mpp;
	struct pm8xxx_mpp_config_data	config;
};

#define PM8XXX_GPIO_INIT(_gpio, _dir, _buf, _val, _pull, _vin, _out_strength, \
			_func, _inv, _disable) \
{ \
	.gpio	= PM8921_GPIO_PM_TO_SYS(_gpio), \
	.config	= { \
		.direction	= _dir, \
		.output_buffer	= _buf, \
		.output_value	= _val, \
		.pull		= _pull, \
		.vin_sel	= _vin, \
		.out_strength	= _out_strength, \
		.function	= _func, \
		.inv_int_pol	= _inv, \
		.disable_pin	= _disable, \
	} \
}

#define PM8XXX_MPP_INIT(_mpp, _type, _level, _control) \
{ \
	.mpp	= PM8921_MPP_PM_TO_SYS(_mpp), \
	.config	= { \
		.type		= PM8XXX_MPP_TYPE_##_type, \
		.level		= _level, \
		.control	= PM8XXX_MPP_##_control, \
	} \
}

#define PM8XXX_GPIO_DISABLE(_gpio) \
	PM8XXX_GPIO_INIT(_gpio, PM_GPIO_DIR_IN, 0, 0, 0, PM_GPIO_VIN_S4, \
			 0, 0, 0, 1)

#define PM8XXX_GPIO_OUTPUT(_gpio, _val) \
	PM8XXX_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_HIGH, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8XXX_GPIO_INPUT(_gpio, _pull) \
	PM8XXX_GPIO_INIT(_gpio, PM_GPIO_DIR_IN, PM_GPIO_OUT_BUF_CMOS, 0, \
			_pull, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_NO, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8XXX_GPIO_OUTPUT_FUNC(_gpio, _val, _func) \
	PM8XXX_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_HIGH, \
			_func, 0, 0)

#define PM8XXX_GPIO_OUTPUT_VIN(_gpio, _val, _vin) \
	PM8XXX_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, _vin, \
			PM_GPIO_STRENGTH_HIGH, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

extern struct gpio_regulator_platform_data
	msm_gpio_regulator_pdata[] __devinitdata;

extern struct msm_bus_scale_pdata mdp_bus_scale_pdata;
extern struct msm_panel_common_pdata mdp_pdata;
extern struct msm_camera_device_platform_data msm_camera_csi_device_data[];

extern struct regulator_init_data msm_saw_regulator_pdata_s5;
extern struct regulator_init_data msm_saw_regulator_pdata_s6;

extern struct rpm_regulator_platform_data msm_rpm_regulator_pdata __devinitdata;
extern struct lcdc_platform_data dtv_pdata;
extern struct msm_camera_gpio_conf msm_camif_gpio_conf;
extern struct platform_device hdmi_msm_device;
extern struct platform_device android_usb_device;
extern struct platform_device msm_tsens_device;

extern struct msm_otg_platform_data msm_otg_pdata;

extern void msm8960_init_hdmi(struct platform_device *hdmi_dev,
						struct msm_hdmi_platform_data *hdmi_data);

extern void __init msm8960_init_usb(void (*vbus_power)(bool on));
extern void __init msm8960_init_dsps(void);

extern void __init msm8960_init_hsic(void);

extern void __init msm8960_init_buses(void);
extern int  __init gpiomux_init(bool use_mdp_vsync);

extern void __init msm8960_init_rpm(void);
extern void __init msm8960_init_regulators(void);

extern void __init msm8960_i2c_init(unsigned speed);
extern void __init msm8960_spi_init(struct msm_spi_platform_data *pdata, 
					struct spi_board_info *binfo, unsigned size);
extern void __init msm8960_gfx_init(void);
extern void __init msm8960_spm_init(void);
extern void __init msm8960_add_common_devices(int (*detect_client)(const char *name));
extern void __init pm8921_gpio_mpp_init(struct pm8xxx_gpio_init *pm8921_gpios,
									unsigned gpio_size,
									struct pm8xxx_mpp_init *pm8921_mpps,
									unsigned mpp_size);
extern void __init msm8960_init_mmc(unsigned sd_detect);
extern void __init msm8960_init_slim(void);
extern void __init msm8960_pm_init(unsigned wakeup_irq);
extern void __init pm8921_init(struct pm8xxx_keypad_platform_data *keypad,
								int mode, int cool_temp, int warm_temp);

extern int  __init msm8960_change_memory_power(u64 start, u64 size, int change_type);
extern void __init msm8960_map_io(void);
extern void __init msm8960_reserve(void);
extern void __init msm8960_allocate_memory_regions(void);
extern void __init msm8960_early_memory(void);

#ifdef CONFIG_ARCH_MSM8930
extern void msm8930_map_io(void);
#endif

extern void msm8960_init_irq(void);

extern int pm8xxx_set_led_info(unsigned index, struct led_info *linfo);

#define PLATFORM_IS_CHARM25() \
	(machine_is_msm8960_cdp() && \
		(socinfo_get_platform_subtype() == 1) \
	)

#endif
