obj-m:=kernel_code.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	gcc user_code.c -o user_code
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	find . -type f -executable -delete