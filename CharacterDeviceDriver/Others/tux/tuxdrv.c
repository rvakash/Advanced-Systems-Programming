#include <linux/module.h> /* for modules */
#include <linux/fs.h> /* file_operations */
#include <linux/uaccess.h> /* copy_(to,from)_user */
#include <linux/init.h> /* module_init, module_exit */
#include <linux/slab.h> /* kmalloc */
#include <linux/cdev.h> /* cdev utilities */

#define driver_name "tuxdrv"
#define majNum 700
#define minNum  0

static char *tuxdrv;
const int bufsize = 100;
// char *buf;
static int __init tux_init(void);
static void __exit tux_exit(void);
static int tux_open(struct inode *inode, struct file *file);
static int tux_release(struct inode *inode, struct file *file);
static ssize_t tux_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos);
static ssize_t tux_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos);

struct file_operations f_op = {
	.owner = THIS_MODULE,
	.open = tux_open,
	.release = tux_release,
	.read = tux_read,
	.write = tux_write,
};

static struct cdev *cdev;

//MODULE_LICENSE(“GPL v2”);
static dev_t first;

static int __init tux_init(void){
	tuxdrv = (char *)kmalloc(bufsize, GFP_KERNEL);
	first = MKDEV(majNum, minNum);
	register_chrdev_region(first, 1, driver_name);
	cdev = cdev_alloc();
	cdev_init(cdev, &f_op);
	cdev_add(cdev, first, 1);
	printk(KERN_INFO "Loading...\n");
	return 1;
}

static void __exit tux_exit(void){
	cdev_del(cdev);
	unregister_chrdev_region(MKDEV(majNum, minNum), 1);
	kfree(tuxdrv);
	printk(KERN_INFO "Unloading..\n");
}

static int tux_open(struct inode *inode, struct file *file){
	pr_info(" OPENING device: %s:\n\n", driver_name);
	return 0;
}

static int tux_release(struct inode *inode, struct file *file){
	pr_info(" CLOSING device: %s:\n\n", driver_name);
	return 0;
}

static ssize_t tux_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos){
	int nbytes;
	if ((lbuf + *ppos) > bufsize) {
		pr_info("trying to read past end of device,""aborting because this is just a stub!\n");
//		return 0;
		pr_info("1Changed code");
		pr_info("lbuf = %d\n", (int) lbuf);
		lbuf = bufsize - *ppos;
		pr_info("lbuf = %d\n", (int) lbuf);
	}
	// pr_info("position = %s\n", tuxdrv + *ppos);
	pr_info("2Changed code");
	nbytes = lbuf - copy_to_user(buf, tuxdrv + *ppos, lbuf);
	*ppos += nbytes;
	pr_info("\n READING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
	return nbytes;
}

static ssize_t tux_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos){
	int nbytes;
	if ((lbuf + *ppos) > bufsize) {
		pr_info("trying to read past end of device,""aborting because this is just a stub!\n");
		return 0;
	}
	nbytes = lbuf - copy_from_user(tuxdrv + *ppos, buf, lbuf);
	*ppos += nbytes;
	pr_info("\n WRITING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
	return nbytes;
}

module_init(tux_init);
module_exit(tux_exit);