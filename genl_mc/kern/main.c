#include <linux/kernel.h>
#include <linux/module.h>
#include <net/genetlink.h>
#include <linux/ioport.h>

#include <linux/wait.h>
#include <linux/kthread.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <linux/delay.h>

#include "genl_event.h"

struct task_struct *thr_ts1;

static char nl_string[50] = "Hello";


int thread_func(void *data) {
 while (!kthread_should_stop ()) {
  sprintf(nl_string, "Hello from kernel at jiffies %ld",jiffies);
  genl_exmpl_event_send(nl_string,50);
  set_current_state (TASK_INTERRUPTIBLE);
        schedule_timeout (cputime_to_jiffies(secs_to_cputime (5))); //Do not schedule this thread again for the specified number of seconds
 }
 return 0;
}

static int __init gnKernel_init(void) {
 printk("Generic Netlink Example Module inserted.\n");

 genl_exmpl_event_init();
 
 thr_ts1 = kthread_run(thread_func, NULL, "kthread1");

 return 0;
}

static void __exit gnKernel_exit(void) {
 kthread_stop(thr_ts1);
 genl_exmpl_event_deinit();

 printk("Generic Netlink Example Module unloaded.\n");
}

module_init(gnKernel_init);
module_exit(gnKernel_exit);
MODULE_LICENSE("GPL");