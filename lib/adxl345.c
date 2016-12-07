#include <linux/i2c-dev.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/stat.h>

#include "adxl345.h"
#include "i2c.h"

/**
 * @function accel_setup
 * @desc configure accel
 */
int adxl345_setup(int fd) {
	/* Set address of the device we wish to speak to */
	if (ioctl(fd, I2C_SLAVE, ADXL345_ADDRESS) < 0) {
		printk(KERN_ERR "GY80 Module: Unable to get bus access to talk to adxl345\n");

		return -1;
	}

	/* Set the acceleration sensor into continious read mode */
	/* approx 3Hz */
	if (i2c_write_reg(fd, 0x2C, 0x06) < 0) return -1;

	/* Data mode */
	if (i2c_write_reg(fd, 0x31, 0x02) < 0) return -1;

	/* Power up */
	if (i2c_write_reg(fd, 0x31, 0x07) < 0) return -1;

	/* Calibration values - I made these up for my sensor! */
	if (i2c_write_reg(fd, 0x1E,   88) < 0) return -1;
	if (i2c_write_reg(fd, 0x1F,   60) < 0) return -1;
	if (i2c_write_reg(fd, 0x20, -127) < 0) return -1;

	return 0;
}

/**
 * @function accel_read
 * @desc get accel indications (warning: memory unsafe)
 */
void adxl345_read(int fd, int* x_o, int* y_o, int* z_o) {
	short int x, y, z;
	unsigned char buf[6];

	/* Set address of the device we wish to speak to */
	if (ioctl(fd, I2C_SLAVE, ADXL345_ADDRESS) < 0) {
		printk(KERN_ERR "GY80 Module: Unable to get bus access adxl345\n");
	}

	/* Read the registers */
	if (i2c_read(fd, 0x32 | 0x80, buf, 6) < 0) {
		printk(KERN_ERR "GY80 Module: Unable to read from adxl345\n");
	}

	x = (buf[1] <<8) | buf[0];
	y = (buf[3] <<8) | buf[2];
	z = (buf[5] <<8) | buf[4];

	*x_o = x;
	*y_o = y;
	*z_o = z;
}