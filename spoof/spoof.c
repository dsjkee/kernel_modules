#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/skbuff.h>
#include <linux/random.h>
#include <net/ip.h>

#define MY_STRING "HTTP"
#define FLAGS_MY 	0x8180
#define TYPE_MY		0x0001
#define CLASS_MY	0x0001
#define GOOGLE_ADDR	0x08080808
#define ANSW_NAME	0xc00c
#define TTL_MY		651

struct dnshdr {
	u16 	id;
	u16		flags;
	u16		quest;
	u16		answ_rr;
	u16		auth_rr;
	u16		add_rr;
} __attribute__((__packed__));


/* 
 * Before this structure you need to place 
 * domen name
 */
struct dns_query {
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


MODULE_LICENSE("GPL");
/*
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
*/
int len_count(unsigned char * buffer)
{
	int i = 0;
	while((*buffer) != 0)
	{	
		buffer++;
		i++;
	}
	return i;
}

void fill_answer_packet(struct dns_answer *answer)
{
	answer->name = htons(ANSW_NAME);
	answer->type = htons(TYPE_MY);
	answer->class = htons(CLASS_MY);
	answer->ttl = htonl(TTL_MY);
	answer->d_len = htons(4);
	answer->addr = htonl(GOOGLE_ADDR);
}

unsigned int my_hook_func(const struct nf_hook_ops *ops,
			       struct sk_buff *skb,
			       const struct net_device *in,
			       const struct net_device *out,
			       int (*okfn)(struct sk_buff *))
{
	struct iphdr *ip, *ip_out;
	struct udphdr *udp, *udp_out;
	void *data, *data_out;
	struct dnshdr *req, *req_out;
	struct sk_buff *my_buffer;
	struct dns_query *query;
	struct dns_answer *answer;
	int name_size;


//	printk(KERN_INFO"HOOK \n");
	ip = ip_hdr(skb);
	if((ip->protocol == 17))
	{
		udp = (struct udphdr *)(skb_transport_header(skb));
	/*	
		printk(KERN_INFO"dest = %d source = %d iphdrlen = %d\n", ntohs(udp->dest), \
		ntohs(udp->source), ip->ihl*4);
	*/
		if(htons(udp->dest) == 53)
		{
			printk(KERN_INFO"<1>DNS\n");
			req = (struct dnshdr *)((unsigned char *)udp + sizeof(struct udphdr));
		//	printk(KERN_INFO"flags = 0x%x\nid = 0x%x\n",  ntohs(req->flags),  ntohs(req->id));
			if(ntohs(req->quest) == 1)
			{
				data = (unsigned char *)req + sizeof(struct dnshdr);
				name_size = len_count((unsigned char *)data);
				query = (struct dns_query *)((unsigned char *)data + name_size);
				
				printk(KERN_INFO"name_len = %d !%s!\n", name_size, (char *)data);
		

				

				my_buffer = alloc_skb(ETH_FRAME_LEN + ntohs(ip->tot_len) + sizeof(struct dns_answer) + 8 \
				, GFP_KERNEL);
				skb_reserve(my_buffer, ETH_FRAME_LEN + 8 + ntohs(ip->tot_len));

				answer = (struct dns_answer *)skb_put(my_buffer, sizeof(struct dns_answer));
				fill_answer_packet(answer);
				printk(KERN_INFO"answer->ttl = %u\n",ntohl(answer->ttl));

				data_out = skb_push(my_buffer, sizeof(struct dns_query) + name_size);
				strncpy(data_out, data, sizeof(sizeof(struct dns_query) + name_size));

				req_out = (struct dnshdr *)skb_push(my_buffer, sizeof(struct dnshdr));
				*req_out = *req;
				req_out->flags = htons(FLAGS_MY);
				req_out->answ_rr = htons(1);
				req_out->add_rr = 0;
				req_out->auth_rr = 0;
//goto metka;
				udp_out = (struct udphdr *)skb_push(my_buffer, 8);
				udp_out->source = udp->dest;
				udp_out->dest = udp->source;
				udp_out->len = htons(8 + sizeof(struct dns_answer) + ntohs(udp->len));
				udp_out->check = csum_tcpudp_magic(ip->saddr, ip->daddr, sizeof(struct dnshdr) \
				+ sizeof(struct dns_answer) + sizeof(struct dns_query) + name_size, IPPROTO_UDP, \
				my_buffer->csum);

				printk(KERN_INFO"udp_csum = 0x%x\n", htons(udp_out->check));

				ip_out = (struct iphdr *)skb_push(my_buffer, ip->ihl * 4);
				(*ip_out) = *ip;
				ip_out->id += 0x31;
				ip_out->saddr = ip->daddr;
				ip_out->daddr = ip->saddr;
				ip_out->tot_len = htonl(ntohl(ip->tot_len) + sizeof(struct dns_answer)/4);
				ip_out->check = 0;
				ip_out->check = ip_fast_csum(ip_out, (ip_out->ihl) * 4);

				printk(KERN_INFO"ip_csum = 0x%x \n", htons(ip_out->check));

				my_buffer->ip_summed = CHECKSUM_COMPLETE;


				if ((dev_queue_xmit(my_buffer)) < 0)
					printk(KERN_INFO"Some ERROR\n");

				kfree(my_buffer);
			}
		}

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