#include <linux/i2c-dev.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/stat.h>

#include "l3g4200d.h"
#include "i2c.h"

/**
 * @function l3g4200d_setup
 * @desc configure l3g4200d
 */
int l3g4200d_setup(int fd) {
	/* Set address of the device we wish to speak to */
	if (ioctl(fd, I2C_SLAVE, L3G4200D_ADDRESS) < 0) {
		printk(KERN_ERR "GY80 Module: Unable to get bus access to talk to l3g4200d\n");

		return -1;
	}

	/* Normal mode, 100 Hz, all axis active */
	if (i2c_write_reg(fd, 0x20, 0x0F) < 0) return -1;
	if (i2c_write_reg(fd, 0x21, 0x00) < 0) return -1;
	if (i2c_write_reg(fd, 0x22, 0x08) < 0) return -1;
	if (i2c_write_reg(fd, 0x23, 0x30) < 0) return -1; 
	if (i2c_write_reg(fd, 0x24, 0x00) < 0) return -1; 

	return 0;
}

/**
 * @function l3g4200d_read
 * @desc get l3g4200d indications
 */
void l3g4200d_read(int fd, int* x_o, int* y_o, int* z_o) {
	short int x,y,z;
	unsigned char buf[6];

	/* Set address of the device we wish to speak to */
	if (ioctl(fd, I2C_SLAVE, L3G4200D_ADDRESS) < 0) {
		printk(KERN_ERR "GY80 Module: Unable to get bus access to l3g4200d\n");
	}

	if (i2c_read(fd, 0x28 | 0x80, buf, 6) < 0) {
		printk(KERN_ERR "GY80 Module: Unable to read from l3g4200d\n");
	}

	x = (buf[1] <<8) | buf[0];
	y = (buf[3] <<8) | buf[2];
	z = (buf[5] <<8) | buf[4];

	*x_o = x-5;
	*y_o = y+2;
	*z_o = z-7;
}