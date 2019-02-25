/*
  Example Linux kernel module, adds a virtual memory area (VMA).
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lawlor@alaska.edu");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.0");


int my_fault(struct vm_fault *vmf) {
	printk(KERN_INFO "Got page fault at user address %lx",
		vmf->address);

	// If we allocated a struct page *,
	// then we could 
	// vm_insert_page(vmf->vma,vmf->address,page);
	
	return VM_FAULT_SIGBUS; //<- kills program
}

static const struct vm_operations_struct my_vm_ops={
	.fault=my_fault
};

static int my_mmap(struct file *f, struct vm_area_struct *vma)
{
	printk(KERN_INFO "Adding VMA");
	vma->vm_ops=&my_vm_ops;
	return 0;
}

struct file_operations my_file_ops = {
	.owner = THIS_MODULE,
	.mmap = my_mmap
};
struct proc_dir_entry *pe;

static int __init my_init(void) {
 printk(KERN_INFO "Hello, kernel mm subsystem!\n");
 pe=proc_create("vmafile",0777, NULL, &my_file_ops);
 return 0; // 0 means "no error".
}
static void __exit my_exit(void) {
 printk(KERN_INFO "Goodbye, kernel mode...\n");
 proc_remove(pe);
}

module_init(my_init);
module_exit(my_exit);

