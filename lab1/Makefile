NAME=mod
KDIR?= /lib/modules/$(shell uname -r)/build

obj-m += mod.o
$(NAME)-objs := src/$(NAME).o src/list.o

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

