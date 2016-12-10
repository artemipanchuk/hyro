#ifndef ADXL345_H
#define ADXL345_H

#define ADXL345_ADDRESS 0x53

int  adxl345_setup(int fd);
void adxl345_read(int fd, int* x_o, int* y_o, int* z_o);

#endif