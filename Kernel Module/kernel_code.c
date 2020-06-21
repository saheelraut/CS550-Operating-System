#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h> // for character driver support
#include <linux/uaccess.h>
#include <linux/sched.h>  // for each process
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/sched/signal.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 1000
#define DEVICE_NAME "process_list"
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Saheel Raut");
MODULE_DESCRIPTION("A module to print process details");
MODULE_VERSION("1.0");

int dev_open(struct inode *inode, struct file *fileptr);
int dev_close(struct inode *inode, struct file *fileptr);
ssize_t dev_read(struct file *fileptr, char __user *buffer, size_t count, loff_t *offp);
char *fetchState(long);

struct task_struct *curr_proc;
int numberofOpens;
char buffer[256];

//File Ops for character device
struct file_operations my_fops = {
    .open = dev_open,
    .read = dev_read,
    .release = dev_close};

//Missolonus device
static struct miscdevice process_list = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &my_fops,
    .mode = 0666};

char *fetchState(long state)
{
    switch (state)
    {
    case 0:
        return "TASK_RUNNING";
    case 1:
        return "TASK_INTERRUPTIBLE";
    case 2:
        return "TASK_UNINTERRUPTIBLE";
    case 3:
        return "TASK_INTERRUPTIBLE , TASK_UNINTERRUPTIBLE";
    case 4:
        return "__TASK_STOPPED";
    case 8:
        return "__TASK_TRACED";
    case 15:
        return "TASK_NORMAL , __TASK_STOPPED , __TASK_TRACED";
    case 16:
        return "EXIT_DEAD";
    case 32:
        return "EXIT_ZOMBIE";
    case 63:
        return "TASK_RUNNING , TASK_INTERRUPTIBLE , TASK_UNINTERRUPTIBLE , __TASK_STOPPED , __TASK_TRACED , EXIT_ZOMBIE , EXIT_DEAD";
    case 64:
        return "TASK_DEAD";
    case 96:
        return "EXIT_ZOMBIE , EXIT_DEAD";
    case 128:
        return "TASK_WAKEKILL";
    case 130:
        return "TASK_WAKEKILL , TASK_UNINTERRUPTIBLE";
    case 132:
        return "TASK_WAKEKILL , __TASK_STOPPED";
    case 136:
        return "TASK_WAKEKILL , __TASK_TRACED";
    case 256:
        return "TASK_WAKING";
    case 512:
        return "TASK_PARKED";
    case 1024:
        return "TASK_NOLOAD";
    case 1026:
        return "TASK_UNINTERRUPTIBLE , TASK_NOLOAD";
    case 2048:
        return "TASK_NEW";
    case 4096:
        return "TASK_STATE_MAX";
    default:
        return "INVALID";
    }
}

// Called during module installation
int __init init_module(void)
{
    if (misc_register(&process_list) < 0)
    {
        printk(KERN_ALERT "Device Registration failed\n");
    }
    curr_proc = next_task(&init_task);

    return 0;
}
// Called when a module is removed
void __exit cleanup_module()
{
    misc_deregister(&process_list);
    printk(KERN_ALERT "Device sucessfully removed\n");
    return;
}

ssize_t dev_read(struct file *file, char *buffer, size_t count, loff_t *offp)
{
    int status;
    struct task_struct *task;
    char process[BUFFER_SIZE];
    printk(KERN_INFO "Process List-> Reading the Prcoess Information...\n");

    for_each_process(task)    //iterating through list of perocesses
    {
        if (curr_proc == task)
        {
            char *state = fetchState(task->state);
            printk(process, "PID=%d PPID=%d CPU=%d STATE=%s", task->pid, task->parent->pid, task_cpu(task), state); // task_cpu -> currently assigned cpu for the process 
            sprintf(process, "PID=%d PPID=%d CPU=%d STATE=%s", task->pid, task->parent->pid, task_cpu(task), state);// task_cpu -> currently assigned cpu for the process
            printk(KERN_INFO "Process: %s", process);
            status = copy_to_user(buffer, process, strlen(process)); //copying block of data to user space
            if (status != 0)
            {
                printk(KERN_ALERT "error copying data to the user");
                return -EFAULT; //bad address macro
            }
            curr_proc = next_task(task);
            break;
        }
    }
    return strlen(process);
}

int dev_open(struct inode *inode, struct file *file)
{
    numberofOpens++;
    printk(KERN_INFO "Process list -> Device has been opened %d time(s)\n", numberofOpens);
    curr_proc = next_task(&init_task);
    return 0;
}

int dev_close(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Process list -> Device successfully closed\n");
    curr_proc = &init_task;
    return 0;
}
