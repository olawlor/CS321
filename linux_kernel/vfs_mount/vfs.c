/*
  Example Linux kernel module, registers a filesystem
  
  https://www.tldp.org/LDP/khg/HyperNews/get/fs/vfstour.html
  https://elixir.bootlin.com/linux/latest/source/include/linux/fs.h#L2162
  https://elixir.bootlin.com/linux/latest/source/security/inode.c#L26
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lawlor@alaska.edu");
MODULE_DESCRIPTION("A simple filesystem of type 'my'.");
MODULE_VERSION("0.0");

// The kernel calls this to fill a "super_block"
static int my_fill_super(struct super_block *sb, void *data, int silent)
{
	static const struct tree_descr files[] = {{""}};
	int error;

  printk(KERN_INFO "Myfs fill_super called\n");
	error = simple_fill_super(sb, 0xBEEF, files);
	if (error)
		return error;

	return 0;
}

// The kernel calls this to mount a filesystem of this type:
static struct dentry *my_mount(struct file_system_type *fs_type,
		  int flags, const char *dev_name,
		  void *data)
{
  printk(KERN_INFO "Myfs mount called: '%s'\n",dev_name);
  return mount_single(fs_type,flags,data,my_fill_super);
  
}

// Unmount (cleanup) filesystem
static void my_unmount(struct super_block *b)
{
  printk(KERN_INFO "Myfs umount called\n");
}

static struct file_system_type my_fs_type = {
  .owner = THIS_MODULE,
  .name="my",
  .mount=my_mount,
  .kill_sb=my_unmount,
};

static int __init my_init(void) {
  printk(KERN_INFO "Registering my filesystem with VFS\n");
  register_filesystem(&my_fs_type);
  return 0; 
}
static void __exit my_exit(void) {
  printk(KERN_INFO "Leaving\n");
}

module_init(my_init);
module_exit(my_exit);

