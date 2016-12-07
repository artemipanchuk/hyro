#include <linux/i2c-dev.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include "i2c.h"

/**
 * @function i2c_seek
 */
void i2c_seek(int fd, unsigned char offset) {
	if ((sys_write(fd, &offset, 1)) != 1) {
		printk(KERN_ERR "GY80 Module: Error writing to i2c slave %2x",offset);
	}
}

/**
 * @function i2c_write_reg
 */
int i2c_write_reg(int fd, unsigned char reg, unsigned char val) {
	char buf[2];
	buf[0] = reg;
	buf[1] = val;

	if ((sys_write(fd, buf, 2)) != 2) {
		printk(KERN_ERR "GY80 Module: Error writing to i2c slave %2x %2x\n", reg, val);
	}

	return 0;
}

/**
 * @function i2c_read
 */
int i2c_read(int fd, unsigned char offset, unsigned char *buf, unsigned char len) {
	i2c_seek(fd, offset);
	
	if (sys_read(fd, buf, len) != len) {
		printk(KERN_ERR "GY80 Module: Unable to read at offset 0x%2X\n", offset);
	}

	return 0;
}