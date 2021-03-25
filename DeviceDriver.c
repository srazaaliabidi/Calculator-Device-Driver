#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h> //this is the same thing as kmalloc.h
#include <linux/uaccess.h> //going to use this to copy to and from user
#include <linux/ioctl.h> 

#define WR_VALUE _IOW('a', 'a', int32_t*) //following manpages for usage of the ioctl.h function
#define RD_VALUE _IOR('a', 'b', int32_t*)

//init our values so that we can tell which is which when we check the kernel
int32_t val1 = 0;
int32_t val2 = 0; 
int32_t val = 0;
int32_t op = 0;
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev; //created the struct to hold the device driver

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations fops = { //tried to keep the file ops simple and easy to read
	.owner = THIS_MODULE,
	.unlocked_ioctl = etx_ioctl,
};

static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	static int count = 0;
	switch(cmd){
		case WR_VALUE: //basis of the write value
			if(count == 0){
				copy_from_user(&op, (int32_t*)arg,sizeof(op));
                		printk(KERN_INFO "op = %d\n",op);
				break;
			}
			else if(count == 1){ //count for the calculator
				copy_from_user(&val1, (int32_t*)arg, sizeof(val1));
				printk(KERN_INFO "val1 = %d\n", val1);
				break;
			}
			else if(count == 2){
				copy_from_user(&val2, (int32_t*)arg, sizeof(val2));
				printk(KERN_INFO "val2 = %d\n", val2);
				break;
			}
		case RD_VALUE:
			if(op == 1)
				val = val1 + val2;
			else if(op == 2)
				val = val1 - val2;
			else if(op == 3)
				val = val1 * val2;
			else if(op == 4)
				val = val1 / val2;
			else
				break;
			copy_to_user((int32_t*)arg, &val, sizeof(val));
			break;
		}
		count += 1;
			if(count == 3)
				count = 0;
	return 0;
}

static int __init etx_driver_init(void){
	if((alloc_chrdev_region(&dev, 0, 1, "etx_dev")) < 0){
		printk(KERN_INFO "cannot allocate major number\n");
		return -1;
	}
	printk(KERN_INFO " MAJOR = %d MINOR = %d\n", MAJOR(dev), MINOR(dev));

	cdev_init(&etx_cdev, &fops);

	if((cdev_add(&etx_cdev, dev, 1)) < 0){
		printk(KERN_INFO "cannot add device to system\n");
		goto r_class;
	}
	
	if((dev_class = class_create(THIS_MODULE, "etx_class")) == NULL){
		printk(KERN_INFO "Cannot create the struct class!\n");
		goto r_class;
	}
	//making the device
	if((device_create(dev_class, NULL, dev, NULL, "etx_device")) == NULL){
		printk(KERN_INFO "Cannot create the Device!\n");
		goto r_device;
	}
	printk(KERN_INFO "Device Driver Insert... DONE\n");
	
	return 0;
	r_device:
		class_destroy(dev_class);
	r_class:
		unregister_chrdev_region(dev, 1);
		return -1;
}

void __exit etx_driver_exit(void){ //this is to show the exit values in the kernel as well as exit from it
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&etx_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Device Driver Remove in progress... Done\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");

