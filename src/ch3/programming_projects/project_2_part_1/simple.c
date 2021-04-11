#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

/* ******* My includes ******* */
#include <linux/sched.h> // `for_each_process()`, `struct task_struct`.


/* This function is called when the module is loaded. */
int simple_init(void)
{
    struct task_struct *t = NULL;
    printk(KERN_INFO "Dijkstra Loading Module\n");

    printk(KERN_INFO "S PID CMD\n");
    for_each_process(t) {
        printk(KERN_INFO "%ld %d %s\n", t->state, t->pid, t->comm);
    }

    return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
    printk(KERN_INFO "Dijkstra Removing Module\n");
}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

