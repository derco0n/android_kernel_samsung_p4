/*
 * arch/arm/mach-tegra/board-p4-sensors.c
 *
 * Copyright (c) 2011, NVIDIA CORPORATION, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of NVIDIA CORPORATION nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/delay.h>
#include <linux/i2c.h>
#include <mach/gpio.h>
#include <mach/gpio-p3.h>
#include <linux/i2c/ak8975.h>
#include <linux/bh1721fvc_p3.h>
#include "gpio-names.h"
#include <linux/mpu.h>

/* we use a skeleton to provide some information needed by MPL
 * but we don't use the suspend/resume/read functions so we
 * don't initialize them so that mldl_cfg.c doesn't try to
 * control it directly.  we have a separate mag driver instead.
 */

static struct mpu3050_platform_data p3_mpu3050_pdata = {
	.int_config  = 0x10,
	/* Orientation for MPU.  Part is mounted rotated
	 * 90 degrees counter-clockwise from natural orientation.
	 * So X & Y are swapped and Y is negated.
	 */
	.orientation = {  0, -1,  0,
			  1,  0,  0,
			  0,  0,  1 },
	.level_shifter = 0, /* single power mode */
	.accel = {
		.get_slave_descr = NULL,
		.irq         = 0,
		.adapt_num   = 0,
		.bus         = EXT_SLAVE_BUS_SECONDARY,
		.address     = 0x0F,
		/* Orientation for the Acc.  Part is mounted rotated
		 * 180 degrees from natural orientation.
		 * So X & Y are both negated.
		 */
		.orientation = { -1,  0,  0,
				  0, -1,  0,
				  0,  0,  1 },
	},
	.compass = {
		.get_slave_descr = NULL,
		.irq	     = 0,
		.adapt_num   = 12,            /*bus number 12*/
		.bus         = EXT_SLAVE_BUS_PRIMARY,
		.address     = 0x0C,
		/* Orientation for the Mag.  Part is mounted rotated
		 * 90 degrees counter-clockwise from natural orientation.
		 * So X & Y are swapped and Y is negated.
		 */
		.orientation = {  0, -1,  0,
				  1,  0,  0,
				  0,  0,  1 },
	},
};


static void p3_mpu3050_init(void)
{
	tegra_gpio_enable(GPIO_MPU_INT);
	gpio_request(GPIO_MPU_INT, "mpu3050_int");
	gpio_direction_input(GPIO_MPU_INT);
}
static const struct i2c_board_info p3_i2c_mpu_sensor_board_info[] = {
	{
		I2C_BOARD_INFO("mpu3050", 0x68),
		.irq = TEGRA_GPIO_TO_IRQ(GPIO_MPU_INT),
		.platform_data = &p3_mpu3050_pdata,
	},
	{
		I2C_BOARD_INFO("kxtf9", 0x0F),
	},
};

static const struct i2c_board_info p3_i2c2_board_info[] = {
	{
		I2C_BOARD_INFO("bq20z75-battery", 0x0B),
	},
};

static int  bh1721fvc_light_sensor_reset(void)
{
#if 0
	int err;

	err = gpio_direction_output(GPIO_LIGHT_SENSOR_DVI, 0);
	if (err) {
		printk(KERN_ERR "Failed to make the light sensor gpio(dvi)"
			" low (%d)\n", err);
		return err;
	}
	udelay(2);
	err = gpio_direction_output(GPIO_LIGHT_SENSOR_DVI, 1);
	if (err) {
		printk(KERN_ERR "Failed to make the light sensor gpio(dvi)"
			" high (%d)\n", err);
		return err;
	}
#endif
	return 0;
}

static struct bh1721fvc_platform_data bh1721fvc_pdata = {
	.reset = bh1721fvc_light_sensor_reset,
};

static struct i2c_board_info p3_i2c_light_sensor_board_info[] = {
	{
		I2C_BOARD_INFO("bh1721fvc", 0x23),
		.platform_data = &bh1721fvc_pdata,
	},
};

static int p3_light_sensor_init(void)
{
#if 0
	int err;

	tegra_gpio_enable(GPIO_LIGHT_SENSOR_DVI);
	err = gpio_request(GPIO_LIGHT_SENSOR_DVI, "LIGHT_SENSOR_DVI");
	if (err) {
		printk(KERN_ERR "Failed to request the light "
			" sensor gpio (%d)\n", err);
		return err;
	}
	err = gpio_direction_output(GPIO_LIGHT_SENSOR_DVI, 1);
	if (err) {
		printk(KERN_ERR "Failed to make the light sensor gpio(dvi)"
			" high (%d)\n", err);
		return err;
	}
#endif
	return 0;
}

static void p3_ak8975_init(void)
{
	tegra_gpio_enable(GPIO_AK8975_INT);
	gpio_request(GPIO_AK8975_INT, "ak8975_int");
	gpio_direction_input(GPIO_AK8975_INT);
}

static struct akm8975_platform_data akm8975_pdata = {
		.gpio_data_ready_int = GPIO_AK8975_INT,
};

static struct i2c_board_info p3_i2c_compass_sensor_board_info[] = {
	{
		I2C_BOARD_INFO("ak8975c", 0x0C),
		.irq = TEGRA_GPIO_TO_IRQ(GPIO_AK8975_INT),
		.platform_data = &akm8975_pdata,
	},
};

int __init p3_sensors_init(void)
{
	p3_light_sensor_init();
	p3_ak8975_init();
	p3_mpu3050_init();

	i2c_register_board_info(0, p3_i2c_mpu_sensor_board_info,
		ARRAY_SIZE(p3_i2c_mpu_sensor_board_info));
	i2c_register_board_info(12, p3_i2c_compass_sensor_board_info,
		ARRAY_SIZE(p3_i2c_compass_sensor_board_info));
	i2c_register_board_info(5, p3_i2c_light_sensor_board_info,
		ARRAY_SIZE(p3_i2c_light_sensor_board_info));

	return 0;
}

