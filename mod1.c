#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

static int a = 0;
static int b = 0;

module_param(a, int, S_IRUGO);
module_param(b, int, S_IRUGO);

static int add(int a, int b)
{
	printk("<1>Function ADD is used %d \n", a+b);
	return a+b;
}

EXPORT_SYMBOL(add);

static int __init start(void)
{
	printk(KERN_INFO"MOD1 say you HELLO! %d\n",add(a,b));
	return 0;
}

static void __exit finish(void)
{
	printk(KERN_INFO"END\n");
}
module_init(start);
module_exit(finish);