#ifndef BMP085_H
#define BMP085_H

#define BMP085_ADDRESS 0x77

int  bmp085_setup(int fd);
void bmp085_read(int fd, long* t_o, long* p_o, float* a_o);

#endif