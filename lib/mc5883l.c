#include <linux/i2c-dev.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/syscalls.h>
#include <linux/delay.h>

#include "mc5883l.h"
#include "i2c.h"

/**
 * @function mc5883l_setup
 * @desc configure mc5883l
 */
int mc5883l_setup(int fd) {
	/* Set address of the device we wish to speak to */
	if (sys_ioctl(fd, I2C_SLAVE, MC5883L_ADDRESS) < 0) {
		printk(KERN_ERR "GY80 Module: Unable to get bus access to talk to mc5883l\n");

		return -1;
	}

	/* Set the mc5883l into continious read mode */
	if (i2c_write_reg(fd, 2, 0) < 0)
		return -1;

	return 0;
}

/**
 * @function mc5883l_pressure
 * @desc configure magnetometer
 */
void mc5883l_read(int fd, int* x_o, int* y_o, int* z_o) {
	char buf[6];
	short int x,y,z;

	/* Set address of the device we wish to speak to */
	if (sys_ioctl(fd, I2C_SLAVE, MC5883L_ADDRESS) < 0) {
		printk(KERN_ERR "GY80 Module:Unable to get bus access to mc5883l\n");
	}

	i2c_seek(fd, 3);

	/* Wait for conversion, should realy be monitoring status register */
	udelay(10000);

	/* Read the mc5883l registers */
	if (sys_read(fd, buf, 6) != 6){
		printk(KERN_ERR "GY80 Module: Unable to read from mc5883l\n");
	}

	/* Display the register values */
	x = (buf[0] <<8) | buf[1];
	y = (buf[2] <<8) | buf[3];
	z = (buf[4] <<8) | buf[5];

	*x_o = x;
	*y_o = y;
	*z_o = z;
}