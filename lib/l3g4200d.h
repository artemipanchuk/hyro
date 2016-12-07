#ifndef L3G4200D_H
#define L3G4200D_H

#define L3G4200D_ADDRESS 0x69

int  l3g4200d_setup(int fd);
void l3g4200d_read(int fd, int* x_o, int* y_o, int* z_o);

#endif