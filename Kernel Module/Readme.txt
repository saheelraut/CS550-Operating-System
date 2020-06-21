Process List Kernel Module:

Instructions to execute:
sudo su 		// Assume role of superuser for execution
make 			// Compile kernel module and user program
insmod kernel_code.ko   // Install kernel module
./user_code 		// run user program 



Optional:
make clean   		//clean project and executable files
lsmod        		// list kernel modules
rmmod <modulename> 	//remove module