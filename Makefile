MODSRC=/root/hyro/libs/gy80

obj-m := gy80.o
gy80-objs := lib/syscalls_wrapper.o lib/i2c.o lib/driver.o lib/adxl345.o lib/l3g4200d.o gy80_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=${MODSRC} modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=${MODSRC} clean