#include <net/genetlink.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "genl_event.h"

//Code based on http://people.ee.ethz.ch/~arkeller/linux/multi/kernel_user_space_howto-3.html

static struct genl_family genl_exmpl_event_family = {
 .id = GENL_ID_GENERATE,         //Genetlink should generate an id
 .hdrsize = 0,
 .name = GENL_EXMPL_EVENT_FAMILYNAME,        //The name of this family, used by userspace application
 .version = GENL_EXMPL_EVENT_VERSION,          //Version number  
 .maxattr = GENL_EXMPL_EVENT_A_MAX,
};

static struct genl_multicast_group genl_exmpl_event_group = {
 .name = GENL_EXMPL_EVENT_GROUPNAME,
};

uint8_t genl_exmpl_event_init(void) {
 int rc;

    //Step 01: Register the new family
 rc = genl_register_family(&genl_exmpl_event_family);
 if (rc != 0) {
  goto failure;
 }

 //Step 02: Register group for the new family
 //Referred: http://lxr.free-electrons.com/source/drivers/acpi/event.c?v=2.6.33#L254
 rc = genl_register_mc_group(&genl_exmpl_event_family, &genl_exmpl_event_group);
 if (rc != 0) {
  printk("Generic Netlink register group: %i",rc);
  genl_unregister_family(&genl_exmpl_event_family);
  goto failure;
 }

 return 0; 
failure:
 return -1;
}

void genl_exmpl_event_deinit(void) {
 int ret;

    //Unregister the family
 ret = genl_unregister_family(&genl_exmpl_event_family);
 if(ret !=0) {
  printk("Generic Netlink unregister family %i\n",ret);
 }
}

int genl_exmpl_event_send(u8 *msg, int len) {
 //Referred: http://lxr.free-electrons.com/source/drivers/acpi/event.c?v=2.6.33#L183
 struct sk_buff *skb;
 int rc;
 void *msg_head;

 //Send a message back ti userspace
    //Allocate some memory, since the size is not yet known use NLMSG_GOODSIZE
 skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
 if (skb == NULL) {
  printk("Could not allocate skb\n");
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
 genlmsg_end(skb, msg_head);

    //Send the message
 rc = genlmsg_multicast(skb, 0,genl_exmpl_event_group.id,GFP_ATOMIC);
 if (rc != 0) {
  //printk("genlmsg_multicast() returned error (Group ID: %d)\n", genl_exmpl_event_group.id);
  return 0;
 } else {
  printk("genlmsg_multicast() event message sent (Group ID: %d): %s\n", genl_exmpl_event_group.id, msg);
 }
 return 0;
}