# obj-m specifies we're a kernel module.
obj-m := reno_abe.o

# Set the path to the Kernel build utils.
KBUILD=/lib/modules/$(shell uname -r)/build/
 
default:
	$(MAKE) -C $(KBUILD) M=$(PWD) modules

clean:
	$(MAKE) -C $(KBUILD) M=$(PWD) clean
