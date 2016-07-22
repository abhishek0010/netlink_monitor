#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/genetlink.h>
#include <unistd.h>

#define GENLMSG_DATA(glh)       ((void *)(NLMSG_DATA(glh) + GENL_HDRLEN))
#define NLA_DATA(na)            ((void *)((char *)(na) + NLA_HDRLEN))


/* Protocol family, consistent in both kernel prog and user prog. */
#define MYPROTO NETLINK_USERSOCK
/* Multicast group, consistent in both kernel prog and user prog. */
#define GROUP_ID 29

typedef unsigned int u32;

static inline u32 netlink_group_mask(u32 group)
{
    return group ? 1 << (group - 1) : 0;
}

int open_netlink(void)
{
    int sock;
    struct sockaddr_nl addr;
    int group = GROUP_ID;

    sock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_GENERIC);
    if (sock < 0) {
        printf("sock < 0.\n");
        return sock;
    }

    memset((void *) &addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    /* This doesn't work for some reason. See the setsockopt() below. */
    addr.nl_groups = netlink_group_mask(GROUP_ID);

    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        printf("bind < 0.\n");
        return -1;
    }

    //_family_id = genl_get_family_id(_sock, "DOC_EXMPL");

    /*
     * 270 is SOL_NETLINK. See
     * http://lxr.free-electrons.com/source/include/linux/socket.h?v=4.1#L314
     * and
     * http://stackoverflow.com/questions/17732044/
     */
/*    if (setsockopt(sock, 270, NETLINK_ADD_MEMBERSHIP, &group, sizeof(group)) < 0) {
        printf("setsockopt < 0\n");
        return -1;
    }
    int group2 = 0;
    if (setsockopt(sock, 270, NETLINK_ADD_MEMBERSHIP, &group2, sizeof(group2)) < 0) {
        printf("setsockopt < 0\n");
        return -1;
    }
*/
    return sock;
}

typedef struct msgtemplate {
    struct nlmsghdr n;
    struct genlmsghdr g;
    char buf[65536];
}msgtemplate_t;

void read_event(int sock)
{
    //struct sockaddr_nl nladdr;
    msgtemplate_t msg;
    //struct iovec iov;
    //char buffer[65536];
    int ret;
    struct nlattr *na;

    /*iov.iov_base = (void *) buffer;
    iov.iov_len = sizeof(buffer);
    msg.msg_name = (void *) &(nladdr);
    msg.msg_namelen = sizeof(nladdr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
*/
    printf("Ok, listening.\n");
    ret = recv(sock, &msg, sizeof(msg), 0);
    if (ret < 0)
        printf("ret < 0.\n");
    else
    {
        na = (struct nlattr *) GENLMSG_DATA(&msg);
        printf("Received message payload: %s\n", (char*)NLA_DATA(na));
    }
}

int main(int argc, char *argv[])
{
    int nls;

    nls = open_netlink();
    if (nls < 0)
        return nls;

    while (1)
        read_event(nls);

    return 0;
}
