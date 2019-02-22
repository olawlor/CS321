/*
  Example Linux kernel module, registers a block device disk.
  
  https://lwn.net/Articles/736534/
  https://elixir.bootlin.com/linux/v4.20.11/source/drivers/block/brd.c
  https://elixir.bootlin.com/linux/v4.20.11/source/drivers/block/loop.c
  https://blog.superpat.com/2010/05/04/a-simple-block-driver-for-linux-kernel-2-6-31/
  https://lwn.net/Articles/58720/
  https://www.oreilly.com/library/view/linux-device-drivers/0596005903/ch16.html
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>
#include <linux/bio.h>
#include <linux/highmem.h>
#include <linux/mutex.h>
#include <linux/radix-tree.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/backing-dev.h>

#ifndef SECTOR_SHIFT
#define SECTOR_SHIFT		9
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lawlor@alaska.edu");
MODULE_DESCRIPTION("A simple block device Linux module.");
MODULE_VERSION("0.0");

// Define a custom global struct to hold our info:
struct my_blockydev {
  unsigned long size;
  spinlock_t lock;
  u8 *data;
  struct gendisk *gd;
  int major;
};
static struct my_blockydev *global_dev;
static const int hardsect_size=4096;


// Actually move the real data:
int my_transfer(struct my_blockydev *dev, struct page *page,
    unsigned int nbytes, unsigned int mem_off, unsigned int op,
		sector_t disk_sector)
{
	u8 *data = dev->data + (disk_sector<<SECTOR_SHIFT);
	void *mem = kmap_atomic(page);
  
  if (op_is_write(op))
    memcpy(data, mem+mem_off, nbytes);
  else
    memcpy(mem+mem_off, data, nbytes);
  
	kunmap_atomic(mem);
	
  return 0;
}

// Interface with the hideously complex bio subsystem
static blk_qc_t my_queue_request(struct request_queue *q, struct bio *bio)
{
	struct my_blockydev *dev = bio->bi_disk->private_data;
	struct bio_vec bvec;
	sector_t sector;
	struct bvec_iter iter;

	sector = bio->bi_iter.bi_sector;
	if (bio_end_sector(bio) > get_capacity(bio->bi_disk))
		goto io_error;

	bio_for_each_segment(bvec, bio, iter) {
		unsigned int len = bvec.bv_len;
		int err;

		err = my_transfer(dev, bvec.bv_page, len, bvec.bv_offset,
				  bio_op(bio), sector);
		if (err)
			goto io_error;
		sector += len >> SECTOR_SHIFT;
	}

	bio_endio(bio);
	return BLK_QC_T_NONE;
io_error:
	bio_io_error(bio);
	return BLK_QC_T_NONE;
}

static struct block_device_operations my_blocky_ops = {
    .owner           = THIS_MODULE,
    //.ioctl	     = sbd_ioctl
};

static int __init my_init(void) {

  // Allocate our struct
  struct my_blockydev *dev=kmalloc(sizeof(struct my_blockydev), 0);
  if (dev==0) return -ENOMEM;
  memset(dev,0,sizeof(*dev)); // zero unused fields
  spin_lock_init(&dev->lock);

  // Allocate storage space
  dev->size=1024*1024*64; // 64MB should be enough for anyone
  dev->data=vmalloc(dev->size);
  if (dev->data==0) return -ENOMEM;
  
  // We need a block device major number:
  dev->major = register_blkdev(dev->major,"blocky");  
  if (dev->major<=0) return -EFAULT;  

  // We make a gendisk
  dev->gd=alloc_disk(16); // subdisk count
  if (dev->gd==0) return -ENOMEM;
  global_dev=dev;
  dev->gd->private_data=dev;
  
  dev->gd->major=dev->major;
  dev->gd->first_minor=0;
  dev->gd->fops=&my_blocky_ops;
  strncpy(dev->gd->disk_name,"blocky0",sizeof(dev->gd->disk_name));
  set_capacity(dev->gd,dev->size/(1L<<SECTOR_SHIFT));
  
  dev->gd->queue=blk_alloc_queue(GFP_KERNEL);
  if (dev->gd->queue==0) return -ENOMEM;
  
	blk_queue_make_request(dev->gd->queue, my_queue_request);
  blk_queue_physical_block_size(dev->gd->queue, hardsect_size);
  blk_queue_max_hw_sectors(dev->gd->queue, dev->size/hardsect_size);
  
  add_disk(dev->gd);
  printk(KERN_INFO "Set up /dev/blocky\n");

  return 0; // 0 means "no error".
}
static void __exit my_exit(void) {
  struct my_blockydev *dev=global_dev;
  if (dev) {
    printk(KERN_INFO "Removing blocky device\n");
    del_gendisk(dev->gd);
    put_disk(dev->gd);  
    unregister_blkdev(dev->major,"blocky");
    vfree(dev->data);  dev->data=0;
  }
}

module_init(my_init);
module_exit(my_exit);

