#include <linux/i2c-dev.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include "driver.h"

#include "syscalls_wrapper.h"
#include "i2c.h"
#include "adxl345.h"
#include "l3g4200d.h"

long fd;

int ax, ay, az;
int gx, gy, gz;
int mx, my, mz;

/**
 * @function setup_driver
 */
int setup_driver(void) {
	int result;

	/* Open i2c-0 device */
	fd = sys_open("/dev/i2c-0", O_RDWR, 0);

	if (fd < 0) {
		printk(KERN_ERR "GY80 Module: Unable to open i2c device\n");
		
		return 1;
	}

	result = adxl345_setup(fd);

	if (result != 0) {
		printk(KERN_ERR "GY80 Module: Unable to setup adxl345\n");
		
		return 1;
	}

	result = l3g4200d_setup(fd);

	if (result != 0) {
		printk(KERN_ERR "GY80 Module: Unable to setup l3g4200d\n");
		
		return 1;
	}

	return 0;
}

void update_driver(void) {
	adxl345_read(fd,  &ax, &ay, &az);
	l3g4200d_read(fd, &gx, &gy, &gz);
}

char* read_driver(void) {
	char* result = kmalloc(64, GFP_KERNEL);
	
	sprintf(
		result, 
		"%5d %5d %5d %5d %5d %5d",
		ax, ay, az, gx, gy, gz
	);

	return result;
}