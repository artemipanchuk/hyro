#include <linux/i2c-dev.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/stat.h>

#include "bmp085.h"
#include "i2c.h"

struct p_calib_data {
	short ac1,ac2,ac3;
	unsigned short ac4,ac5,ac6;
	short b1,b2,mb,mc,md;
} p_calib;

/**
 * @function pressure_setup
 * @desc configure barometer
 */
int bmp085_setup(int fd) {
	char data[22];

	/* Set address of the device we wish to speak to */
	if (ioctl(fd, I2C_SLAVE, BMP085_ADDRESS) < 0)  {
		printk(KERN_ERR "GY80 Module: Unable to get bus access to talk to bmp085\n");

		return -1;
	}

	i2c_seek(fd, 0xAA);
	
	if (read(fd, data, 22) != 22){
		printk(KERN_ERR "GY80 Module: Unable to read calibration data from bmp085\n");

		return -1;
	}
	
	p_calib.ac1 = (data[ 0]<<8) | data[ 1];
	p_calib.ac2 = (data[ 2]<<8) | data[ 3];
	p_calib.ac3 = (data[ 4]<<8) | data[ 5];
	p_calib.ac4 = (data[ 6]<<8) | data[ 7];
	p_calib.ac5 = (data[ 8]<<8) | data[ 9];
	p_calib.ac6 = (data[10]<<8) | data[11];
	p_calib.b1  = (data[12]<<8) | data[13];
	p_calib.b2  = (data[14]<<8) | data[15];
	p_calib.mb  = (data[16]<<8) | data[17];
	p_calib.mc  = (data[18]<<8) | data[19];
	p_calib.md  = (data[20]<<8) | data[21];
	
	return 0;
}

/**
 * @function read_pressure
 * @desc configure accel
 */
void bmp085_read(int fd, long* t_o, long* p_o, float* a_o) {
	unsigned char oss = 3;
	unsigned char buf[3];

	long ut,up;
	long x1,x2,x3;
	long b3,b5,b6;
	unsigned long b4,b7;
	long t,p;
	float alt;

	/* Set address of the device we wish to speak to */
	if (ioctl(fd, I2C_SLAVE, BMP085_ADDRESS) < 0) {
		printk(KERN_ERR "GY80 Module: Unable to get access to bmp085\n");
	}

	/* Start conversion to get temperature */
	if (i2c_write_reg(fd, 0xF4, 0x2E) < 0)
		exit(1);

	usleep(4500);

	i2c_seek(fd, 0xF6);
	if (read(fd, buf, 2) != 2){
		printk(KERN_ERR "GY80 Module: Unable to read from bmp085\n");
	}

	ut = (buf[0] <<8) | buf[1];

	if (i2c_write_reg(fd, 0xF4, 0x34 + (oss <<6)) < 0)
		exit(1);

	/* Changes depending on value of oss */
	usleep(25500);

	i2c_seek(fd, 0xF6);
	if (read(fd, buf, 3) != 3){
		printk(KERN_ERR "GY80 Module: Unable to read from bmp085\n");
	}

	/* Display the register values */
	up = ((buf[0] <<16) | (buf[1]<<8) | buf[2]) >> (8-oss);

	/* Calculate the true temp */
	x1 = (ut - p_calib.ac6) * p_calib.ac5 / (1<<15);
	x2 = (p_calib.mc *(1<<11)) / (x1 + p_calib.md);
	b5 = x1+x2;
	t = (b5+8)/(1<<4);

	/* Calc pressure */
	b6 = b5 - 4000;
	x1 = (p_calib.b2*((b6*b6)>>12))>>11;
	x2 = (p_calib.ac2*b6)>>11;
	x3 = x1+x2;
	b3 = (((p_calib.ac1*4+x3)<<oss)+2)/4;
	x1 = (p_calib.ac3*b6)>>13;
	x2 = (p_calib.b1*(b6*b6)>>12)>>16;
	x3 = ((x1+x2)+2)>>2;
	b4 = (p_calib.ac4*((unsigned long)x3+32768))>>15;
	b7 = ((unsigned long)up-b3)*(50000>>oss);

	if (b7 < 0x80000000)
		p = (b7*2)/b4;
	else
		p = (b7/b4)*2;

	x1 = (p>>8)*(p>>8);
	x1 = (x1*3038)>>16;
	x2 = (-7357*p)>>16;
	p = p + ((x1+x2+3781)>>4);
	alt = 44330.0 * (1-pow(p/101325.0,1.0/5.255));

	*t_o = t/10;
	*p_o = p;
	*a_o = alt;
}