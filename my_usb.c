#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h>
#define NAME_DEVICE "denis_usb"
#define NAME_CLASS "denis_class"
MODULE_LICENSE("GPL");


static struct usb_device_id table[] = {
	{USB_DEVICE(0x067b, 0x2303)},
	{}
};
MODULE_DEVICE_TABLE(usb, table);


static ssize_t my_read(struct file f*, char __user buf*, size_t, loff_t lof*)
{

}

static ssize_t my_write(struct file *, const char __user *, size_t, loff_t *)
{

}
static struct file_operations f_opr = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write =my_write,
};

static struct usb_class_driver my_class = {
	.name = NAME_CLASS,
	.fops = &f_opr,
};

static struct usb_device *my_device;

static int my_probe (struct usb_interface *intf, \
		      const struct usb_device_id *id)
{
	int i;
	struct usb_host_interface *cur_host_intf = intf->altsetting;
	struct usb_endpoint_descriptor *endpoint;
	printk(KERN_INFO"This device by vendor %d and product %d\n", \
		id->idVendor, id->idProduct);

	for(i = 0; i < cur_host_intf->desc.bNumEndpoints; ++i)
	{
		endpoint = &cur_host_intf->endpoint[i].desc;

		printk(KERN_INFO"EP[%d] Address: 		%d\n", i, endpoint->bEndpointAddress);
		printk(KERN_INFO"EP[%d] Attribute:		%d\n", i, endpoint->bmAttributes);
		printk(KERN_INFO"EP[%d] MaxPacketSize:  %d\n", i, endpoint->wMaxPacketSize);
	}

	my_device = interface_to_usbdev(intf);


	return 0;
}

static void my_disconnect (struct usb_interface *intf)
{
	printk(KERN_INFO"Device Disconnect\n");
}

static struct usb_driver my_usb_driver = {
		.name = NAME_DEVICE,
		.probe = my_probe,
		.disconnect = my_disconnect,
		.id_table = table,
};

static int __init start(void)
{
	int res;
	res = usb_register(&my_usb_driver);
	if (res > -1)
		printk(KERN_INFO"Device was registrated\n");
	return 0;
}

static void __exit finish(void)
{
	usb_deregister(&my_usb_driver);
	printk(KERN_INFO"Driver was deregistrated\n");
}
module_init(start);
module_exit(finish);