MODSRC=/root/hyro/libs/gy80
EXTRA_CFLAGS := -I/root/hyro/libs

obj-m := gy80.o
gy80-objs := lib/driver.o lib/ahrs.o lib/i2c.o lib/adxl345.o lib/l3g4200d.o lib/mc5883l.o lib/bmp085.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=${MODSRC} modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=${MODSRC} clean