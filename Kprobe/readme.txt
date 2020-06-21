Kprobe:

Instructions to execute:
sudo su 		                 // Assume role of superuser for execution
make 			                 // Compile  module
insmod kernel_code.ko pid=<pid>  // Install module
dmesg 		                     // see logs



Optional:
lsmod        		// list kernel modules
rmmod <modulename> 	//remove module


Saheel Raut
B00814913