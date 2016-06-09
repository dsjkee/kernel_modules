#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

extern int add(int,int);

static int a1;
static int b1;

module_param(a1, int, S_IRUGO);
module_param(b1, int, S_IRUGO);


static int __init start(void)
{
	printk(KERN_INFO"MOD2 say you HELLO! %d\n",add(a1,b1));
	return 0;
}

static void __exit finish(void)
{
	printk(KERN_INFO"END\n");
}
module_init(start);
module_exit(finish);