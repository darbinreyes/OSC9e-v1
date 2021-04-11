#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/list.h> // list
#include <linux/types.h> // list_head
#include <linux/slab.h> // kmalloc(), kfree()

struct birthday {
  char *name;
  int year;
  struct list_head list;
};

static LIST_HEAD(birthday_list);

/* This function is called when the module is loaded. */
int simple_init(void)
{
  struct birthday *tn, *ptr;
  char *names_arr[] = {"Edsgar Dijkstra", "Tony Hoare", "Donald Knuth", "Derek Decker", "Euclid"};
  int years_arr[] = {1965, 1977, 1979, 1981, 323};
  int i;
  printk(KERN_INFO "Loading Darbin's Module\n");

  // Create linked list of 5 nodes.
  printk(KERN_INFO "Creating list.\n");
  for (i = 0; i < 5; i++) {
    tn = kmalloc(sizeof(*tn), GFP_KERNEL);
    tn->name = names_arr[i];
    tn->year = years_arr[i];
    INIT_LIST_HEAD(&tn->list);
    list_add_tail(&tn->list, &birthday_list);
  }

  // Print list elements.
  printk(KERN_INFO "Traversing list.\n");
  list_for_each_entry(ptr, &birthday_list, list) {
    printk(KERN_INFO "%s %d.\n", ptr->name, ptr->year);
  }

  return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
  struct birthday *ptr, *next;
  printk(KERN_INFO "Removing Darbin's Module\n");

  // Freeing list memory.
  list_for_each_entry_safe(ptr, next, &birthday_list, list) {
    printk(KERN_INFO "Deleting %s %d.\n", ptr->name, ptr->year);
    list_del(&ptr->list);
    kfree(ptr);
  }
}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Darbin's Simple Module");
MODULE_AUTHOR("Darbin Reyes");

