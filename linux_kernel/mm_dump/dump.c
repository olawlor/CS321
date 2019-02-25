/*
  Example Linux kernel module, dumps the page table entries.
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

static int bad_address(void *p)
{
	unsigned long dummy;

	return probe_kernel_address((unsigned long *)p, dummy);
}

static void dump_pagetable(unsigned long address)
{
	pgd_t *base = __va(read_cr3_pa());
	pgd_t *pgd = base + pgd_index(address);
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	if (bad_address(pgd))
		goto bad;

	pr_info("PGD %lx ", pgd_val(*pgd));

	if (!pgd_present(*pgd))
		goto out;

	p4d = p4d_offset(pgd, address);
	if (bad_address(p4d))
		goto bad;

	pr_cont("P4D %lx ", p4d_val(*p4d));
	if (!p4d_present(*p4d) || p4d_large(*p4d))
		goto out;

	pud = pud_offset(p4d, address);
	if (bad_address(pud))
		goto bad;

	pr_cont("PUD %lx ", pud_val(*pud));
	if (!pud_present(*pud) || pud_large(*pud))
		goto out;

	pmd = pmd_offset(pud, address);
	if (bad_address(pmd))
		goto bad;

	pr_cont("PMD %lx ", pmd_val(*pmd));
	if (!pmd_present(*pmd) || pmd_large(*pmd))
		goto out;

	pte = pte_offset_kernel(pmd, address);
	if (bad_address(pte))
		goto bad;

	pr_cont("PTE %lx", pte_val(*pte));
out:
	pr_cont("\n");
	return;
bad:
	pr_info("BAD\n");
}


ssize_t my_read(struct file *f, char __user *dest, size_t nbytes, loff_t *where)
{
   ssize_t len;
   char data[100];
dump_pagetable((unsigned long)dest);
   sprintf(data,"Your kernel mem is at %p\n",(dest));
   len=strlen(data);
   printk(KERN_INFO "READ HERE: size %ld, where %ld\n",
	(long)nbytes,(long)*where);
   return simple_read_from_buffer(dest,nbytes,where, data,len);
}

struct file_operations my_file_ops = {
	.owner = THIS_MODULE,
	.read = my_read
};
struct proc_dir_entry *pe;

static int __init my_init(void) {
 printk(KERN_INFO "Hello, fancy kernel mode!\n");
 printk(KERN_INFO "sizeof(long)=%d bytes\n", (int)sizeof(long) );
 pe=proc_create("fancyfile",0777, NULL, &my_file_ops);
 return 0; // 0 means "no error".
}
static void __exit my_exit(void) {
 printk(KERN_INFO "Goodbye, kernel mode...\n");
 proc_remove(pe);
}

module_init(my_init);
module_exit(my_exit);

