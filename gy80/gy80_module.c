#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <linux/fs.h>
#include <linux/miscdevice.h>

#include <asm/uaccess.h>

#include "lib/syscalls_wrapper.h"
#include "lib/driver.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GY80 driver");
MODULE_VERSION("0.1");

static ssize_t gy80_module_read(struct file *file, char *buffer, size_t count, loff_t *ppos) {
	char *driver_answer = read_driver();
	
	/* Don't include the null byte. */
	int length = strlen(driver_answer);

	/* We only support reading the whole string at once */
	if (count < length)
		return -EINVAL;

	if (*ppos != 0)
		return 0;

	if (copy_to_user(buffer, driver_answer, length))
		return -EINVAL;

	/* Tell the user how much data we wrote */
	*ppos = length;

	kfree(driver_answer);

	return length;
}

static const struct file_operations gy80_module_fops = {
	.owner = THIS_MODULE,
	.read  = gy80_module_read,
};

static struct miscdevice gy80_module_dev = {
	MISC_DYNAMIC_MINOR,
	"gy80",
	&gy80_module_fops
};

static int __init gy80_module_init (void) {
	int result;

	printk(KERN_INFO "GY80 Module: initialization started\n");

	/* Register gy80 device */
	result = misc_register(&gy80_module_dev);
	if (result) {
		printk(KERN_ERR "GY80 Module: Unable to register GY80 module misc device\n");
		
		return -1;
	}

	result = setup_syscalls_wrapper();

	if (result == 1) {
		printk(KERN_ERR "GY80 Module: Unable to setup syscalls wrapper\n");

		misc_deregister(&gy80_module_dev);
		
		return -1;
	}

	result = setup_driver();

	if (result == 1) {
		printk(KERN_ERR "GY80 Module: Unable to setup driver\n");

		misc_deregister(&gy80_module_dev);
		
		return -1;
	}

	printk(KERN_INFO "GY80 Module: successfully initialized\n");
	
	return result;
}

static void __exit gy80_module_exit (void) {
	printk(KERN_INFO "GY80 Module: exit started\n");
	
	misc_deregister(&gy80_module_dev);

	printk(KERN_INFO "GY80 Module: successfully exited\n");
}

module_init(gy80_module_init);
module_exit(gy80_module_exit);