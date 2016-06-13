#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <linux/netlink.h>
#include <linux/genetlink.h>
#include <sys/socket.h>

# define MAX_PAYLOAD 2048
#define NETLINK_TEST 31
#define GROUP_ID 17

struct nlmsghdr *nlh;


int open_socket(void)
{
	int user_sock;

	user_sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_TEST);
	if (user_sock < 0) {
        printf("sock < 0.\n");
        return user_sock;
    }

	struct sockaddr_nl src_addr;

	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid();
	/* addr.nl_groups = MYMGRP; This is not working*/
	if(bind(user_sock, (struct sockaddr*)&src_addr,sizeof(src_addr)))
	{
		printf("Bind < 0\n");
		return -1;
	}

	int grp = GROUP_ID;
	//this may be the option to use 
	if (setsockopt(user_sock, 270, NETLINK_ADD_MEMBERSHIP, &grp, sizeof(grp)) < 0) {
        printf("setsockopt < 0\n");
        return -1;
    }

}

void send_message(int sock)
{
	struct sockaddr_nl dest_addr;
	memset(&dest_addr, 0, sizeof(dest_addr));
 	dest_addr.nl_family = AF_NETLINK;

	struct msghdr msg;
	
	struct iovec iov;

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
	sendmsg(sock, &msg, 0);
}

void receive_message(int sock)
{
	struct msghdr msg;
	/* Read message from kernel */
	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	int ret = recvmsg(sock, &msg, 0);
	if (ret < 0)
        printf("ret < 0.\n");
    else
    	printf(" Received message payload: %s\n",NLMSG_DATA(nlh));

}

void close_socket(int sock)
{
	/* Close Netlink Socket */
	close(sock);
}

int main()
{
 	//dest_addr.nl_pid = 0;   /* For Linux Kernel */
 	int nlsoc = open_socket();
 	if (nlsoc<0)
 		return nlsoc;
 	while(1)
 		receive_message(nlsoc);
 	return 0;
}
