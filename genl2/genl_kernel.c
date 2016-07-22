#include <linux/module.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <net/genetlink.h>

enum {
 GENL_EXMPL_EVENT_A_UNSPEC,
 GENL_EXMPL_EVENT_A_MSG,
    __GENL_EXMPL_EVENT_A_MAX,
};
#define GENL_EXMPL_EVENT_A_MAX (__GENL_EXMPL_EVENT_A_MAX - 1)

enum {
 GENL_EXMPL_EVENT_C_UNSPEC,
 GENL_EXMPL_EVENT_C_DO,
 __GENL_EXMPL_EVENT_C_MAX,
};
#define GENL_EXMPL_EVENT_C_MAX (__GENL_EXMPL_EVENT_C_MAX - 1)

#define GENL_EXMPL_EVENT_VERSION 1
#define GENL_EXMPL_EVENT_FAMILYNAME "CONTROL_EXMPL"
#define GENL_EXMPL_EVENT_GROUPNAME "EXMPL_GRP"


//#define GROUP_ID 31

static struct genl_family genl_exmpl_event_family = {
 .id = GENL_ID_GENERATE,         //Genetlink should generate an id
 .hdrsize = 0,
 .name = GENL_EXMPL_EVENT_FAMILYNAME,        //The name of this family, used by userspace application
 .version = GENL_EXMPL_EVENT_VERSION,          //Version number  
 .maxattr = GENL_EXMPL_EVENT_A_MAX,
};

static struct nla_policy doc_exmpl_genl_policy[GENL_EXMPL_EVENT_A_MAX + 1] = {
	[GENL_EXMPL_EVENT_A_MSG] = { .type = NLA_NUL_STRING },
};


static int doc_exmpl_echo(struct sk_buff *skb, struct genl_info *info)
{
	printk("Received a msg from kernel space..");
	return 0;
}

static struct genl_ops doc_exmpl_genl_ops[] = {
	{
		.cmd = GENL_EXMPL_EVENT_C_DO,
		.policy = doc_exmpl_genl_policy,
		.doit = doc_exmpl_echo,
	},
};

static struct genl_multicast_group genl_exmpl_event_group[] = {
	{
		.name = GENL_EXMPL_EVENT_GROUPNAME
	},
};

int genl_exmpl_event_send(u8 *msg, int len) {
 //Referred: http://lxr.free-electrons.com/source/drivers/acpi/event.c?v=2.6.33#L183
 struct sk_buff *skb;
 int rc;
 void *msg_head;
 //unsigned char *msg = "Hello from kernel..";
 //int error;

 //Send a message back ti userspace
    //Allocate some memory, since the size is not yet known use NLMSG_GOODSIZE
 pr_info("Creating Message..\n");
 skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
 if (skb == NULL) {
  printk("Could not allocate skb:- genlmsg_new() failed\n");
  return 0;
 }

 //Create the message headers
    /* arguments of genlmsg_put: 
       struct sk_buff *, 
       int (sending) pid, 
       int sequence number, 
       struct genl_family *, 
       int flags, 
       u8 command index (why do we need this?)
    */
 pr_info("Putting Contents...\n");
 msg_head = genlmsg_put(skb, 0, 0, &genl_exmpl_event_family, 0, GENL_EXMPL_EVENT_C_DO);
 if (msg_head == NULL) {
  rc = -ENOMEM;
  printk("genlmsg_put() returned error\n");
  return 0;
 }

 //Add a GENL_EXMPL_EVENT_A_MSG attribute (actual value to be sent)
 rc = nla_put(skb, GENL_EXMPL_EVENT_A_MSG, len, msg);
 if (rc != 0) {
  printk("nla_put() returned error\n");
  return 0;
 }
 
 //Finalize the message
 pr_info("Ending message..\n");
 genlmsg_end(skb, msg_head);

    //Send the message
 pr_info("Multicasting...\n");
 pr_info("The group ID is %u.\n", genl_exmpl_event_family.mcgrp_offset);
 rc = genlmsg_multicast(&genl_exmpl_event_family, skb, 0, 0, GFP_KERNEL);
 if (rc != 0) {
 	pr_err("genlmsg_multicast_allns() failed: %d\n", rc);
                    pr_err("(This can happen if nobody is listening. "
                                    "Because it's not that unexpected, "
                                    "you might want to just ignore this error.)\n");
                    return 0;
   	}
  pr_info("Success in sending msg from kernel\n");
 return 0;
}


static int init_socket(void)
    {
    	int error;
    	pr_info("Registering family.\n");
    	error = genl_register_family_with_ops_groups(&genl_exmpl_event_family, doc_exmpl_genl_ops, genl_exmpl_event_group);
    	if (error)
    		pr_err("Family registration failed: %d\n", error);

    	return error;
    }

static int __init hello_init(void)
    {
            int error;

            error = init_socket();
            if (error)
                    return error;

            

            pr_info("Hello module registered.\n");

            error = genl_exmpl_event_send((u8)"Hello from Kernel!", 20);

            return 0;
    }

    static void __exit hello_exit(void)
    {
            genl_unregister_family(&genl_exmpl_event_family);
            pr_info("Hello removed.\n");
    }

    module_init(hello_init);
    module_exit(hello_exit);

    MODULE_LICENSE("GPL");




