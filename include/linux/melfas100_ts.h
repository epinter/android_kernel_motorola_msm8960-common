/*
 * include/linux/melfas100_ts.h
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
 */

#ifndef _LINUX_MELFAS100_TS_H
#define _LINUX_MELFAS100_TS_H

#define MELFAS_TS_NAME "melfas-ts"

#define TS_FLIP_X           (1 << 0)
#define TS_FLIP_Y           (1 << 1)

struct melfas_ts_platform_data {
	int x_size;
	int y_size;
	int  version;
	unsigned long	flags;
	int		(*hw_reset)(void);
};

#endif /* _LINUX_MELFAS_TS100_H */
