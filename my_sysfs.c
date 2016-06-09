#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/parport.h>
#include <asm/uaccess.h>
#include <linux/stat.h>

#define CLASS_NAME "class1"
MODULE_LICENSE("GPL");

const char kernel_input[19] = "Hello from kernel\n\0";
char kernel_output[32];
static struct class *my_class;

static ssize_t my_show(struct class *class, \
	struct class_attribute *attr, char *buf)
{
	int res;
	printk(KERN_INFO"my_show %s\n", buf);
	//copy_to_user(buf, kernel_input, 7);
	res = strcpy(buf, kernel_input);
	printk(KERN_INFO"WROTEN TO USERSPACE %zd BYTES\n%s\n", strlen(buf), buf);
	return 0;
}
static ssize_t my_store(struct class *class, struct class_attribute *attr, \
	const char *buf, size_t count)
{
	printk(KERN_INFO"my_store %zd %s\n", count, buf);
	if(count >= 32)
	{
		printk(KERN_INFO"SO BIG MESSAGE\n");
		return 0;
	}
	strncpy(kernel_output, buf, count);
	kernel_output[count] = '\0';
	return 0;
}

CLASS_ATTR(xxx,S_IWUSR | S_IWGRP | S_IRUSR | S_IRGRP,my_show,my_store);

static int __init start(void)
{
	int res;
	my_class = class_create(THIS_MODULE, CLASS_NAME);
	if(my_class < 0)
		printk(KERN_INFO"ERROR CLASS CREATE\n");
	if((res = class_create_file(my_class, &class_attr_xxx)) < 0)
		printk(KERN_INFO"ERROR CLASS FILE CREATE\n");

	printk(KERN_INFO"CLASS CREATE\n");
	return 0;
}

static void __exit finish(void)
{

	class_remove_file(my_class, &class_attr_xxx);
	class_destroy(my_class);
	printk(KERN_INFO"EXIT\n");
}
module_init(start);
module_exit(finish);