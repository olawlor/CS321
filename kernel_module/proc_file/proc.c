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
MODULE_DESCRIPTION("A simple /proc Linux module.");
MODULE_VERSION("0.0");

// This gets called when somebody opens /proc/foo
static int my_proc_open(struct inode *inode, struct file *file)
{
  printk(KERN_INFO "Got open call\n");
  return 0; // <- no error from opening the file
}

// This gets called when somebody reads /proc/foo
static ssize_t my_proc_read(struct file *filp, 
    char __user *buffer,size_t len, 
    loff_t *offset)
{
  static const char *my_str="\n\n** Howdy from kernelspace! **\n\n\n";
  size_t count=strlen(my_str);
 
  printk(KERN_INFO "Got read call with user buffer %p:%zu bytes at offset %d\n",
    buffer,len, (int)(*offset));
  
  if (count>len) count=len; // shrink to the user's size
  if (*offset !=0) count=0; // only read our stuff from the beginning
  
  // Move data out of kernel memory into user's buffer
  if (0!=copy_to_user(buffer, my_str, count))
    return -EFAULT; // something bad about that
  else 
  {
    *offset += count; // move the file down
    return count; // it worked, return byte count
  }
}

// This gets called when somebody closes /proc/foo
static int my_proc_release(struct inode *inode, struct file *file)
{
  printk(KERN_INFO "Got close call\n");
  return 0; // <- no error from closing the file
}

// A file_operations struct says how to do operations on files.
static const struct file_operations my_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = my_proc_open,
    .read       = my_proc_read,
    .release    = my_proc_release,
};

static int __init my_init(void) {
  printk(KERN_INFO "Setting up /proc/foo\n");
  proc_create("foo",0666,NULL,&my_proc_fops);
  return 0; // 0 means "no error".
}
static void __exit my_exit(void) {
  remove_proc_entry("foo",NULL);
  printk(KERN_INFO "Removing /proc/foo\n");
}

module_init(my_init);
module_exit(my_exit);

