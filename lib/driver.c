#include <linux/i2c-dev.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include "i2c.h"
#include "adxl345.h"
#include "bmp085.h"
#include "l3g4200d.h"
#include "mc5883l.h"

#include "ahrs.h"

#include "driver.h"

#define PI 3.14159265359

long fd;

int ax, ay, az;
int gx, gy, gz;
int mx, my, mz;

float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;

/**
 * @function setup_driver
 */
int setup_driver(void) {
	/* Open i2c-0 device */
	fd = sys_open("/dev/i2c-0", O_RDWR, 0);

	if (fd < 0) {
		printk(KERN_ERR "GY80 Module: Unable to open i2c device\n");
		
		return 1;
	}

	/* Setup i2c subdevices */
	mc5883l_setup(fd);
	bmp085_setup(fd);
	adxl345_setup(fd);
	l3g4200d_setup(fd);

	return 0;
}

int update_driver(void) {
	mc5883l_read(fd,  &mx, &my, &mz);
	adxl345_read(fd,  &ax, &ay, &az);
	l3g4200d_read(fd, &gx, &gy, &gz);

	ahrs_update(gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f, ax, ay, az, mx, my, mz, &q0, &q1, &q2, &q3);
}

char* read_driver(void) {
	char* result = kmalloc(64, GFP_KERNEL);
	
	sprintf(
		result,
		"%7.2f %7.2f %7.2f %7.2f",
		q0, q1, q2, q3
	);

	return result;
}