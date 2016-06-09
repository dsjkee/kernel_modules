#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

const char *mod_name = "my_dev";
static int buffer = 0;
const char *to = (char *)&buffer;

ssize_t my_write(struct file *my_file,const char __user *user_buffer, size_t count, loff_t *loff )
{
	printk("<1>USER write");
	get_user(buffer, user_buffer);
	printk(" %d ", buffer);
	buffer = buffer * buffer; 
	printk("%d\n", buffer);
	return 1;
}
ssize_t my_read (struct file *my_file, char __user *user_buffer, size_t count, loff_t *loff)
{
	printk("<1>USER READ %d \n", buffer);
	put_user(buffer, user_buffer);
	return 1;
}

static struct file_operations f_op = 
{
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
};

static int __init start(void)
{
	int maj;
	maj = register_chrdev(250, mod_name, &f_op);
	if(maj < 0)
		printk(KERN_INFO"Some error %d\n", maj);
	return 0;
}

static void __exit finish(void)
{
	unregister_chrdev(250, mod_name);
	printk(KERN_INFO"END\n");
}
module_init(start);
module_exit(finish);