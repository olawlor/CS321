/*
  Example Linux kernel module, does nothing but start and stop.
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lawlor@alaska.edu");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.0");

static int __init my_init(void) {
 printk(KERN_INFO "Hello, kernel mode!\n");
 printk(KERN_INFO "sizeof(long)=%d bytes\n", (int)sizeof(long) );
 return 0; // 0 means "no error".
}
static void __exit my_exit(void) {
 printk(KERN_INFO "Goodbye, kernel mode...\n");
}

module_init(my_init);
module_exit(my_exit);

