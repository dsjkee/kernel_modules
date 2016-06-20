#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/random.h>

#define MY_STRING "HTTP"
#define FLAGS_MY 	0x8180
#define TYPE_MY		0x0001
#define CLASS_MY	0x0001
#define GOOGLE_ADDR	0x08080808
#define ANSW_NAME	0xc00c

struct dnshdr {
	u16 	id;
	u16		flags;
	u16		quest;
	u16		answ_rr;
	u16		auth_rr;
	u16		add_rr;
} __attribute__((__packed__));

struct dns_query {
	char *name;
	u16 type;
	u16 class;
}  __attribute__((__packed__));

struct dns_answer {
	u16		name;
	u16 	type;
	u16		class;
	u32		ttl;
	u16		d_len;
	u32		addr;
} __attribute__((__packed__));

struct dnspack_answer {
	struct dnshdr hdr;
	struct dns_query query;
	struct dns_answer;
}  __attribute__((__packed__));

MODULE_LICENSE("GPL");

static int Rabin_Karp(char *dest, const char *src, int d_size, int s_size)
{
	int i, res, q, s0, si;
	q = 23;
	s0 = (int)(*src)%q;
	for(i = 0; i < d_size - s_size; ++i)
	{
		si = (int)(*(dest + i));
		if(s0 == si)
			res = strncmp(dest + i, src, s_size);
			if(res == 0)
				return 1;
	}
	return 0;
} 

unsigned int my_hook_func(const struct nf_hook_ops *ops,
			       struct sk_buff *skb,
			       const struct net_device *in,
			       const struct net_device *out,
			       int (*okfn)(struct sk_buff *))
{
	struct iphdr *ip;
	struct tcphdr *tcp;
	void *data;
	int tcp_size = 0, res, data_size;

	printk(KERN_INFO"HOOK\n");
	ip = ip_hdr(skb);
	printk(KERN_INFO"%d\n",ip->protocol);
	if(ip->protocol == 6)
	{
		tcp = tcp_hdr(skb);
		tcp_size = tcp_hdrlen(skb);
		data = tcp + tcp_size;
		data_size = ntohs(ip->tot_len) - (ip->ihl) * 4 - tcp_size;
		printk(KERN_INFO"ip_tot_len = %d ip_hdr_len = %d tcp_len = %d\n", ntohs(ip->tot_len), (ip->ihl) * 4, tcp_size);
		printk(KERN_INFO"Data size %d  string size %zd", data_size, strlen(MY_STRING));
		res = Rabin_Karp((char*)data, MY_STRING, data_size, strlen(MY_STRING));
		if(res)
		{
			printk(KERN_INFO"vk.com PACKET\n");
			return NF_DROP;
		}		//printk(KERN_INFO"%s\n", (char*)data);
	}
	return NF_ACCEPT;

}

struct nf_hook_ops my_hook_ops = {
		.hook = my_hook_func,
		.owner = THIS_MODULE,
		.pf = PF_INET,
		.hooknum = NF_INET_PRE_ROUTING,
		.priority = NF_IP_PRI_FIRST,
};
static int __init start(void)
{
	int res;
	res = nf_register_hook(&my_hook_ops);
	if(res < 0)
		printk(KERN_INFO"Some error %d\n", res);
	return 0;
}

static void __exit finish(void)
{
	nf_unregister_hook(&my_hook_ops);
	printk(KERN_INFO"END\n");
}
module_init(start);
module_exit(finish);