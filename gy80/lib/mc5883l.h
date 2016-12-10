#ifndef MC5883L_H
#define MC5883L_H

#define MC5883L_ADDRESS 0x1E

int  mc5883l_setup(int fd);
void mc5883l_read(int fd, int* x_o, int* y_o, int* z_o);

#endif