KERNEL_DIR=/home/long/gitee/ebf/ebf-buster-linux
#KERNEL_DIR=/home/long/gitee/ebf/ebf_6ull_linux
obj-m := globalmem.o
all:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) modules
clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) clean	
