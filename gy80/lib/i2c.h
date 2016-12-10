#ifndef I2C_H
#define I2C_H

void i2c_seek(int fd, unsigned char offset);
int i2c_write_reg(int fd, unsigned char reg, unsigned char val);
int i2c_read(int fd, unsigned char offset, unsigned char *buf, unsigned char len);

#endif