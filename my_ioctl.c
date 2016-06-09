#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include "my_header.h"
MODULE_LICENSE("GPL");

char input[16];
static char output[16] = "from module";

static long my_ioctl(struct file *my_file, unsigned int cmd, unsigned long arg)
{
	int check;
	printk(KERN_INFO"ioctl_active\n");
	switch(cmd)
	{
		case WRITE_TO_MODULE:
			check = copy_from_user(input, (const void *)arg, _IOC_SIZE(cmd));
			if(check < 0)
				printk(KERN_INFO"Error in WRITE_TO_MODULE\n");
			printk(KERN_INFO"%s\n",input);
			break;
		case READ_FROM_MODULE:
			check = copy_to_user((void *)arg, output, _IOC_SIZE(cmd));
			if(check < 0)
				printk(KERN_INFO"Error in READ_FROM_MODULE\n");
			break;
		default:
			printk(KERN_INFO"Default error\n");
	}
	return 1;
}

static struct file_operations f_opt = {
	.owner = THIS_MODULE,
	.unlocked_ioctl =  my_ioctl,
	.compat_ioctl = my_ioctl
};


/*
 *	Module is registating here, before it, we need to initialising operations from
 *	struct file_operations and create file with
 *	name MODULE_NAME by means of "mknod" in some directory with
 *	major number MAJOR_NUMBER and zero's minor number example
 *	" mknod -m0666 ioctl_module c 231 0 " 
 */
static int __init start(void)
{
	int maj;
	maj = register_chrdev(MAJOR_NUMBER, MODULE_NAME, &f_opt);
	if(maj < 0)
		printk(KERN_INFO"Some error %d\n", maj);
	return 0;
}

static void __exit finish(void)
{
	unregister_chrdev( MAJOR_NUMBER, MODULE_NAME);
	printk(KERN_INFO"EXIT\n");
}
module_init(start);
module_exit(finish);