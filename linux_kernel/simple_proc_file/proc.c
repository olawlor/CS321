/*
  Example Linux kernel module, registers a read-only /proc/foo file.
  
  http://www.lifl.fr/~lipari/courses/ase_lkp/ase_lkp.html
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lawlor@alaska.edu");
MODULE_DESCRIPTION("A simpler /proc Linux module.");
MODULE_VERSION("0.0");

// This gets called when somebody reads our /proc/foo
static ssize_t my_proc_read(struct file *filp, 
    char __user *buffer,size_t len, 
    loff_t *offset)
{
  static const char *my_str="\n\n** Howdy from kernelspace! **\n\n\n";
  size_t count=strlen(my_str);
 
  return simple_read_from_buffer(buffer,len,offset, my_str,count);
}

// A file_operations struct says how to do operations on files.
static const struct file_operations my_proc_fops = {
    .owner      = THIS_MODULE,
    .read       = my_proc_read,
};

static int __init my_init(void) {
  printk(KERN_INFO "Setting up /proc/foo\n");
  proc_create("foo",0666,NULL,&my_proc_fops);
  return 0; // 0 means "no error".
}
static void __exit my_exit(void) {
  printk(KERN_INFO "Removing /proc/foo\n");
}

module_init(my_init);
module_exit(my_exit);

