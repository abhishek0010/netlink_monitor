#include <linux/module.h>
#include <linux/kernel.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/netlink.h>
//#include <linux/skbuff.h>
#include <net/net_namespace.h> //new here


#define MYPROTO NETLINK_USERSOCK

#define GROUP_ID 31

static struct sock *nl_sk = NULL;

static void hello_nl_recv_msg(void) 
{
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	//int pid;
	//struct sk_buff *skb_out;
	char *msg="Hello from kernel";
	int msg_size = strlen(msg) + 1;
	int res;

	printk(KERN_INFO "Entering: %s\n", __FUNCTION__);
	//nlh=(struct nlmsghdr*)skb->data;
	//printk(KERN_INFO "Netlink received msg payload:%s\n",(char*)nlmsg_data(nlh));
	//pid = nlh->nlmsg_pid; /*pid of sending process */
	skb = nlmsg_new(NLMSG_ALIGN(msg_size + 1), GFP_KERNEL);//changed 0 to GPF_KERNEL
	if(!skb)
	{
		printk(KERN_ERR "Failed to allocate new skb\n");
		return;
	} 

	nlh = nlmsg_put(skb, 0, 1, NLMSG_DONE, msg_size + 1, 0);
	strcpy(nlmsg_data(nlh),msg); 
	//NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */ 
	printk(KERN_INFO "Sending msg now.....");
	res = nlmsg_multicast(nl_sk, skb, 0, GROUP_ID, GFP_KERNEL); //added this line for making msg multicast
	//working for ports created with python scripts
	//res=nlmsg_unicast(nl_sk,skb_out,pid);

	if(res<0)
    	printk(KERN_INFO "Error while sending bak to user\n");
    else 
    	printk(KERN_INFO "Successful Delivery\n");
}

static int __init hello_init(void)
{
	printk("Entering: %s\n",__FUNCTION__);
	//This is for 3.6 kernels and above.
	/*struct netlink_kernel_cfg cfg = {
		.input = hello_nl_recv_msg,
	};*/
	nl_sk = netlink_kernel_create(&init_net, MYPROTO, NULL);
	//nl_sk = netlink_kernel_create(&init_net, MYPROTO, 0, hello_nl_recv_msg,NULL,THIS_MODULE);
	if(!nl_sk)
	{
    	printk(KERN_ALERT "Error creating socket.\n");
    	return -10;
	}
	hello_nl_recv_msg();
	netlink_kernel_release(nl_sk);
	return 0;
}

static void __exit hello_exit(void) {

printk(KERN_INFO "exiting hello module\n");

}

module_init(hello_init); 
module_exit(hello_exit);

MODULE_LICENSE("GPL");
