obj-m += nl_kernelb.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	#gcc -o nl_user1 nl_user.c 
	gcc -o nl_user2 nl_user3.c
	#python nl_py_user.py

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	#rm nl_user1
	rm nl_user2
