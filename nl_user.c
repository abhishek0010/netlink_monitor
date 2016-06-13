#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <linux/netlink.h>
#include <linux/genetlink.h>
#include <sys/socket.h>

# define MAX_PAYLOAD 2048
#define NETLINK_TEST 31

struct sockaddr_nl src_addr, dest_addr;
struct msghdr msg;
struct nlmsghdr *nlh;
struct iovec iov;
int user_sock;

int main()
{
	user_sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_TEST);
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	bind(user_sock, (struct sockaddr*)&src_addr,sizeof(src_addr));


	memset(&dest_addr, 0, sizeof(dest_addr));
 	dest_addr.nl_family = AF_NETLINK;
 	dest_addr.nl_pid = 0;   /* For Linux Kernel */

 	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
 	/* Fill the netlink message header */
 	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
 	nlh->nlmsg_pid = getpid();  /* self pid */
 	nlh->nlmsg_flags = 0;
 	/* Fill in the netlink message payload */
 	strcpy(NLMSG_DATA(nlh), "Hello you from user 1!");
	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	sendmsg(user_sock, &msg, 0);
	/* Read message from kernel */
	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	recvmsg(user_sock, &msg, 0);
	printf(" Received message payload: %s\n",
	NLMSG_DATA(nlh));
	/* Close Netlink Socket */
	close(user_sock);
}
