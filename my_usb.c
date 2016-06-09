#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h>
#define NAME_DEVICE "denis_usb"
#define NAME_CLASS "denis_class"
#define EP_IN	0x83
#define EP_OUT	0x02
#define PACKET_SIZE 128
MODULE_LICENSE("GPL");


static struct usb_device_id table[] = {
	{USB_DEVICE(0x067b, 0x2303)},
	{}
};
static char buffer[PACKET_SIZE];
MODULE_DEVICE_TABLE(usb, table);
static struct usb_device *my_device;

static ssize_t my_read(struct file *f, char __user *buf, size_t size, loff_t *lof)
{
	int reading, res;
	printk("READ\n");
	res = usb_bulk_msg(my_device, usb_rcvbulkpipe(my_device, EP_IN),\
		buffer,	PACKET_SIZE, &reading, 50);

	if(res)
	{
		printk(KERN_INFO"res = %d %s\n", res, buffer);
	}

	if(reading < size)
		copy_to_user(buf, buffer, reading);
	else
		copy_to_user(buf, buffer, size);
	return 0;
}
int my_open (struct inode *my_node, struct file *my_file)
{
	printk(KERN_INFO"OPEN FILE\n");
	return 0;
}

static struct file_operations f_opr = {
	.owner = THIS_MODULE,
	.read = my_read,
	.open = my_open,
};

static struct usb_class_driver my_class = {
	.name = "usb/den%d",
	.fops = &f_opr,
	.minor_base = 0,
};


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
	i = usb_register_dev(intf, &my_class);
	if (i < 0)
		printk("ERROR\n");
	return 0;
}

static void my_disconnect (struct usb_interface *intf)
{
	printk(KERN_INFO"Device Disconnect\n");
	usb_deregister_dev(intf, &my_class);
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