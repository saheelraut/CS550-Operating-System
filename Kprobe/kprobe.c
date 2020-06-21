#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kprobes.h>
#include <linux/module.h>
#include <asm/current.h>
#include <linux/time.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Saheel Raut");
MODULE_DESCRIPTION("Kprobe");
MODULE_VERSION("1.0");

int pid;
struct timespec time;
module_param(pid, int, 0);

struct task_struct *task;


static struct kprobe kp = {

    .symbol_name = "handle_mm_fault",
};

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{

    if (pid == current->pid)
    {
        time = current_kernel_time();

        printk("<%s> pre_handler: regs->si = 0x%lx,time: %ld\n", p->symbol_name, regs->si,time.tv_nsec);
    }

    return 0;
}


static int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr)
{
    time = current_kernel_time();

    printk("fault_handler: p->addr = 0x%p,time: %ld, trap #%dn", p->addr,time.tv_nsec, trapnr);
    return 0;
}

static int __init kprobe_init_module(void)
{
    int register_probe_flag;
    // Check if pid is passed as argument.
    if (pid == 0)
    {
        printk(KERN_INFO "Please pass PID as an argument");
        return -EINVAL;
    }

    kp.pre_handler = handler_pre;
    //_kprobe.post_handler = k_post_handler;
    kp.fault_handler = handler_fault;

    //registration function such as register_kprobe() specifies where the probe is to be inserted and what handler is to be called when the probe is hit.
    register_probe_flag = register_kprobe(&kp);
    if (register_probe_flag < 0)
    {
        pr_err("register_kprobe failed %d\n", register_probe_flag);
        return -1;
    }
    pr_info("Planted kprobe at %p\n", kp.addr);

    return 0;
}

static void __exit kprobe_exit_module(void)
{
    // Dregister a kprobe
    unregister_kprobe(&kp);
}

module_init(kprobe_init_module)
module_exit(kprobe_exit_module)