/*
  Example Linux kernel module, registers a filesystem
  
  https://www.tldp.org/LDP/khg/HyperNews/get/fs/vfstour.html
  https://lwn.net/Articles/13325/
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

#define MY_MAGIC 0xBEEF /* used to recognize superblocks */

// This is called when somebody reads our example file.
static ssize_t
my_file_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
  char *data = "\n\nEgg-xcelent!\n\n";
  printk(KERN_INFO "Myfs sample file read called at offset %d\n",(int) *ppos);
  return simple_read_from_buffer(buf,nbytes,ppos, data,strlen(data));
}

// These are the operations on our (one!) example file
static const struct file_operations my_file_operations = {
  .owner = THIS_MODULE,
  .read = my_file_read
};

// These are the operations on our superblock
static const struct super_operations my_super_ops = {
  .statfs    = simple_statfs,
  .drop_inode  = generic_delete_inode,
};

// The kernel calls this to fill a "super_block" 
//  (filesystem info) to link us into the VFS layer.
static int my_fill_super(struct super_block *sb, void *data, int silent)
{
  // You fill the superblock with the initial files in the root directory:
  static const struct tree_descr files[] = {
    // [0] and [1] are reserved for . and ..
    [2] = {"eggsample", &my_file_operations, 0666}
  };
  int error;

  printk(KERN_INFO "Myfs fill_super called\n");
  error = simple_fill_super(sb, MY_MAGIC, files);
  if (error)
    return error;
  
  sb->s_op=&my_super_ops;

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

// This struct defines the mount/unmount operations for our filesystem
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
  printk(KERN_INFO "Leaving module\n");
  unregister_filesystem(&my_fs_type);
}

module_init(my_init);
module_exit(my_exit);

