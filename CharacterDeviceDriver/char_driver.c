#include <linux/module.h> /* for modules */
#include <linux/fs.h> /* file_operations */
#include <linux/uaccess.h> /* copy_(to,from)_user */
#include <linux/init.h> /* module_init, module_exit */
#include <linux/slab.h> /* kmalloc */
#include <linux/cdev.h> /* cdev utilities */
#include <linux/version.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/list.h>
#include <linux/string.h>
#include <asm/uaccess.h>

#define driverName "kachu"
#define ramdiskSize (size_t) (16 * PAGE_SIZE)
#define CDEV_IOC_MAGIC 'k'
#define ASP_CLEAR_BUFF _IO(CDEV_IOC_MAGIC, 1)
int numDevices = 3;
int major = 0;
static struct class *kachuClass;

struct asp_kachu {
	struct cdev cdev;
	char *ramdisk;
	struct semaphore sem;
	int devNo;
	// struct list_head list;
	// long acc_dir_md;
};

module_param(numDevices, int, S_IRUGO);
int kachu_open(struct inode *inode, struct file *filp);
int kachu_release(struct inode *inode, struct file *filp);
ssize_t kachu_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t kachu_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
static int kachu_setup_cdev(struct asp_kachu *dev, int minor, struct class *class);
static void kachu_cleanup_module(void);
static int __init kachu_init(void);
static void __exit kachu_exit(void);
loff_t kachu_llseek(struct file *filp, loff_t off, int ori);
long kachu_ioctl(struct file *filp, unsigned int in_cmd, unsigned long dir);

struct file_operations fops = {
	.owner = THIS_MODULE,
	.read =  kachu_read,
	.write = kachu_write,
	.open =  kachu_open,
	.release = kachu_release,
	.llseek = kachu_llseek,
	.unlocked_ioctl = kachu_ioctl,
};

struct asp_kachu *kachuDevices;


/*
 * Open and close
 */
int kachu_open(struct inode *inode, struct file *filp){
	int majNo = imajor(inode);
	int minNo = iminor(inode);
	struct asp_kachu *dev = NULL;
	if (majNo != major || minNo < 0 || minNo >= numDevices){
		printk(KERN_WARNING "No device with minor=%d and major=%d exist\n", minNo, majNo);
		return -ENODEV; 	// no such device
	}
	dev = &kachuDevices[minNo];
	filp->private_data = dev; 
	return 0;
}

int kachu_release(struct inode *inode, struct file *filp){
	return 0;
}

/*
 * Data management: read and write
 */

ssize_t kachu_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos){

	struct asp_kachu *dev = (struct asp_kachu *)filp->private_data;
	unsigned char *tempBuff = NULL;
	int k;
	ssize_t retval = 0;
	if(down_interruptible(&(dev->sem))!=0){
		pr_err("%s: not able to lock semaphore\n", driverName);
	}
	if(count<0 || *f_pos<0){
		retval = -EINVAL;
		goto out;
	}
	if(*f_pos > ramdiskSize){
		retval = -EINVAL;
		goto out;
	}

	tempBuff = (unsigned char*) kzalloc(count, GFP_KERNEL);
	if(tempBuff == NULL){
		printk(" out of memory operation has failed");
		retval = -ENOMEM;
		goto out;
	}
	for(k=0; k<count; k++){
		tempBuff[k] = dev->ramdisk[*f_pos + k];
	}
	*f_pos += count;
	copy_to_user(buf, tempBuff, count);
	retval = count;

out:
	printk("exiting the read!!!\n");
	if (tempBuff != NULL){
		kfree(tempBuff);
	}
	up(&(dev->sem));
	return retval;
}

ssize_t kachu_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos){
	struct asp_kachu *dev = (struct asp_kachu *)filp->private_data;
	ssize_t retval = 0;
	unsigned char *tempBuff = NULL;
	loff_t i;
	if(down_interruptible(&(dev->sem))!=0) {
		pr_err("%s: unable to lock the semaphore\n", driverName);
	}
	if(count < 0 || *f_pos < 0){
		retval = -EINVAL;
		goto out;
	}
	if(*f_pos > ramdiskSize){	
		retval = -EINVAL;
		goto out;
	}

	tempBuff = (unsigned char*) kzalloc(count, GFP_KERNEL);	
	if(tempBuff == NULL){	
		retval = -ENOMEM;
		goto out;
	}
	copy_from_user(tempBuff, buf, count);
		for (i = 0; i < count; i++){
			dev->ramdisk[*f_pos + i] = tempBuff[i];
		}
		*f_pos += count;
	retval = count;
	
out:
	if (tempBuff != NULL)
		kfree(tempBuff);
	up(&(dev->sem));
	return retval;
}

loff_t kachu_llseek(struct file *filp, loff_t off, int ori) {

	struct asp_kachu *dev = (struct asp_kachu *)filp->private_data;
	loff_t new_pos;


	if(down_interruptible(&(dev->sem)) != 0){
		pr_err("%s: not able to lock the semaphore\n", driverName);
	}

	switch(ori){
		case 0:
			new_pos = off;		
			break;
		case 1:
			new_pos = filp->f_pos + off;
			break;
		case 2:
			new_pos = ramdiskSize + off;
			break;
		default:
			new_pos = -EINVAL;
		goto fail;
	}

	if(new_pos < 0 || new_pos > ramdiskSize -1){
		new_pos = -EINVAL;
		goto fail;
	}

	filp->f_pos = new_pos;
fail:
	up(&(dev->sem));
	return new_pos;
	
}

long kachu_ioctl(struct file *filp, unsigned int in_cmd, unsigned long dir) {

	struct asp_kachu *dev = (struct asp_kachu *)filp->private_data;
	// int req_dir;
	// long retval;
	if(in_cmd != ASP_CLEAR_BUFF){
		return -1;
	}
	if(down_interruptible(&(dev->sem))!=0){
		pr_err("%s: not able to lock the semaphore\n", driverName);
	}
	memset((void *)dev->ramdisk, 0, ramdiskSize);
	filp->f_pos=0;	

	// copy_from_user(&req_dir,(int*)acc_dir,sizeof(int)*1);
	
	// if(req_dir != DAD_REGULAR && req_dir != DAD_REVERSE)
	// {
	// 	return -1;
	// }
	
	
	// retval = chr_dev->acc_dir_md;
	// chr_dev->acc_dir_md = req_dir;
	up(&(dev->sem));
	return 1;
}


static int kachu_setup_cdev(struct asp_kachu *dev, int minor, struct class *class){
	dev_t devno = MKDEV(major, minor);
	struct device *device = NULL;
// dev->buffer_size = ramdiskSize;
	dev->devNo = minor;
	dev->ramdisk = NULL;
	dev->cdev.owner = THIS_MODULE;
	sema_init(&(dev->sem),1);
// initialize cdev structure
	cdev_init(&dev->cdev, &fops);
	dev->ramdisk = (unsigned char*) kzalloc(ramdiskSize, GFP_KERNEL);
	cdev_add(&dev->cdev, devno, 1);
	device = device_create(class, NULL, devno, NULL, driverName "%d", minor);

	return 0;
}

static void kachu_cleanup_module(void){
	// struct asp_kachu *dev = NULL;
	int i;
	if (kachuDevices) {
	for(i=0; i<numDevices; i++){
		device_destroy(kachuClass, MKDEV(major, i));
		cdev_del(&kachuDevices[i].cdev);
		kfree(&kachuDevices[i].ramdisk);
	}
	}
	kfree(kachuDevices);
	if (kachuClass)
		class_destroy(kachuClass);
	unregister_chrdev_region(MKDEV(major, 0), numDevices);
	return;
}

static int __init kachu_init(void){
	dev_t dev = 0;
	int i, result;
	i = result = 0;
	
	// if (numDevices <= 0) {
	// 	printk(KERN_WARNING "Invalid number of devices passed = %d\n", numDevices);
	// 	err = -EINVAL;	// Invalid argument
	// 	return err;
	// }
	
	/* Obtain a range minor numbers for the speified number of devices (starting with 0) */
	result = alloc_chrdev_region(&dev, 0, numDevices, driverName);
	major = MAJOR(dev);
	if (result < 0) {
		printk(KERN_WARNING "kachu: cant get major\n");
		return result;
	}
	/* Create device class */
	kachuClass = class_create(THIS_MODULE, driverName);
	// if(IS_ERR(kachuClass)) {
	// 	err = PTR_ERR(kachuClass); 
	// 	goto fail;
	// }
/* 
	 * allocate the devices -- we can't have them static, as the number
	 * can be specified at load time
*/	
	kachuDevices = (struct asp_kachu *)kzalloc(numDevices *sizeof(struct asp_kachu), GFP_KERNEL);
	// if(!kachuDevices) {
	// 	result = -ENOMEM;
	// 	goto fail;
	// }
        /* Initialize each device. */
	for(i=0; i<numDevices; i++){
		result = kachu_setup_cdev(&kachuDevices[i], i, kachuClass);
		if(result){
			// devToDestroy = i;
			goto fail;
		}
	}

	return 0;

fail:
	kachu_cleanup_module();
	return result;
	
}

static void __exit kachu_exit(void){	
	kachu_cleanup_module();
	return;
}

module_init(kachu_init);
module_exit(kachu_exit);
MODULE_AUTHOR("Akash R Vasishta");
MODULE_LICENSE("GPL v2");