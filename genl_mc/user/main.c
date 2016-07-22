#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>

#include <linux/genetlink.h>



/* Code based on
 * http://people.ee.ethz.ch/~arkeller/linux/multi/kernel_user_space_howto-3.html
 * and
 * http://sourcecodebrowser.com/acpid/1.0.10/acpid_2src_2drivers_2netlink_8c_source.html
 * and
 * http://sourcecodebrowser.com/acpid/1.0.10/acpid_2include_2acpid_2driver_2netlink_8h.html
 */



/* Generic macros for dealing with netlink sockets. Might be duplicated
 * elsewhere. It is recommended that commercial grade applications use
 * libnl or libnetlink and use the interfaces provided by the library
 */
#define NLMSG_TAIL(nlh) ((struct nlattr *) (((void *) (nlh)) + NLMSG_ALIGN((nlh)->nlmsg_len)))
#define NLA_OK(rta,len) ((len) >= (int)sizeof(struct nlattr) && \
                (rta)->nla_len >= sizeof(struct nlattr) && \
                (rta)->nla_len <= (len))
#define NLA_NEXT(rta,attrlen)      ((attrlen) -= NLA_ALIGN((rta)->nla_len), \
                               (struct nlattr*)(((char*)(rta)) + NLA_ALIGN((rta)->nla_len)))
#define NLA_LENGTH(len)     (NLA_ALIGN(sizeof(struct nlattr)) + (len))
#define NLA_SPACE(len)      NLA_ALIGN(NLA_LENGTH(len))
#define NLA_DATA(rta)   ((void*)(((char*)(rta)) + NLA_LENGTH(0)))
#define NLA_PAYLOAD(rta) ((int)((rta)->nla_len) - NLA_LENGTH(0))
#define GENLMSG_DATA(glh) ((void *)(NLMSG_DATA(glh) + GENL_HDRLEN))
#define GENLMSG_PAYLOAD(glh) (NLMSG_PAYLOAD(glh, 0) - GENL_HDRLEN)
#define GENL_MAX_FAM_OPS   256
#define GENL_MAX_FAM_GRPS   256


//Variables used for netlink
int nl_fd;  //netlink socket's file descriptor
struct sockaddr_nl nl_address;  //netlink socket address
int nl_family_id; //The family ID resolved by the netlink controller for this userspace program
int nl_group_id;
int nl_rxtx_length; //Number of bytes sent or received via send() or recv()
struct nlattr *nl_na;   //pointer to netlink attributes structure within the payload 
struct {    //memory for netlink request and response messages - headers are included
    struct nlmsghdr n;
    struct genlmsghdr g;
    char buf[256];
} nl_request_msg, nl_response_msg;
unsigned long nl_sequence_number = 0;
static const char *nl_family_name = "CONTROL_EXMPL";
static const char *nl_group_name = "EXMPL_GRP";
#define GENL_EXMPL_EVENT_VERSION 1



//Implementation of common netlink related methods: open, close, send, receive
int netlink_open(int * fd, unsigned long * seq_init, int protocol, int groups) {
    struct sockaddr_nl nladdr;  //netlink socket address
    socklen_t len;
    
    *fd = socket(AF_NETLINK, SOCK_RAW, protocol);
    if (*fd < 0) {
        return -1;
    }
    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_groups = groups;


    if (bind(*fd, (struct sockaddr *)&nladdr, sizeof(nladdr)) < 0) {
        goto fail;
    }
    len = sizeof(nladdr);
    if (getsockname(*fd, (struct sockaddr *)&nladdr, &len) < 0) {
        goto fail;
    }

    *seq_init = time(NULL);

    return 0;
fail:
    close(*fd);
    return -1;
}

int netlink_send(int fd, unsigned long * seq_num, struct nlmsghdr *n, pid_t peer, int groups) {
    struct sockaddr_nl nladdr = { .nl_family = AF_NETLINK, .nl_pid = peer, .nl_groups = groups };
    n->nlmsg_seq = ++(*seq_num);
    return sendto(fd, n, n->nlmsg_len, 0, (struct sockaddr *)&nladdr, sizeof(nladdr));
}

int netlink_recv(int fd, struct nlmsghdr *n, pid_t *peer) {
    struct sockaddr_nl nladdr;
    socklen_t len = sizeof(nladdr);
    int ret = recvfrom(fd, n, n->nlmsg_len, 0, (struct sockaddr *)&nladdr, &len);
    *peer = nladdr.nl_pid;
    return ret;
}


int netlink_wait(int fd, unsigned long *seq_num, struct nlmsghdr *n, pid_t peer) {
    int len = n->nlmsg_len;

    for (;;) {
        pid_t sender;
        struct nlmsghdr * h;
        n->nlmsg_len = len;
        int ret = netlink_recv(fd, n, &sender);
        if (ret < 0)  {
            fprintf(stderr, "%s() | Error\n", __func__);
            continue;
        }
        if (sender != peer) {
            fprintf(stderr, "%s() | Error: Source PID mismatch\n", __func__);
            continue;
        }
        for (h = n; NLMSG_OK(h, ret); h = NLMSG_NEXT(h, ret)) {
            if (h->nlmsg_pid != getpid()) {
                fprintf(stderr, "%s() | Error: Destination PID mismatch\n", __func__);
                continue;
            }

            if (h->nlmsg_type == NLMSG_ERROR) {
                fprintf(stderr, "%s() | Error: Message receive error\n", __func__);
                return -1;
            }
            memcpy(n, h, h->nlmsg_len);
            return 0;
        }
    }
}

void netlink_close(int fd) {
    close(fd);
}

//Implementation of common netlink message parsing methods
int netlink_attr_attach(struct nlmsghdr *n, int max, int type, const void *data, int alen) {
    int len = NLA_LENGTH(alen);
    struct nlattr *nla;

    if (NLMSG_ALIGN(n->nlmsg_len) + NLA_ALIGN(len) > max) {
        return -1;
    }

    nla = NLMSG_TAIL(n);
    nla->nla_type = type;
    nla->nla_len = len;
    memcpy(NLA_DATA(nla), data, alen);
    n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + NLA_ALIGN(len);
    return 0;
}

int netlink_attr_parse(struct nlattr *table[], int max, struct nlattr *src, int len) {
    memset(table, 0, sizeof(struct nlattr *) * (max + 1));
    while (NLA_OK(src, len)) {
        if (src->nla_type <= max)
        table[src->nla_type] = src;
        src = NLA_NEXT(src, len);
    }
    return 0;
}

static int netlink_verify_group(struct nlattr * attr, int * group, const char * expected_group_name) {
    const char *name;
    if (attr == NULL) {
        return -1;
    }

    struct nlattr *attrs[CTRL_ATTR_MCAST_GRP_MAX + 1];
    netlink_attr_parse(attrs, CTRL_ATTR_MCAST_GRP_MAX, NLA_DATA(attr), attr->nla_len - NLA_HDRLEN);

    name = NLA_DATA(attrs[CTRL_ATTR_MCAST_GRP_NAME]);
    if (strcmp(name, expected_group_name)) {
        return -1;
    }

    *group = *(__u32 *) (NLA_DATA(attrs[CTRL_ATTR_MCAST_GRP_ID]));
    return 0;
}

int netlink_wait_for_event(int fd, char * received_msg, int max) {
    char buffer[256];
    int i;
    int ret;
    struct nlmsghdr *n = (struct nlmsghdr *) &buffer;
    struct nlmsghdr * h;
    pid_t sender;

    n->nlmsg_len = 256;

    
    ret = netlink_recv(fd, n, &sender);

    if (ret < 0) {
        return -1;
    }

    i = 0;
    for (h = n; NLMSG_OK(h, ret) && i < max; h = NLMSG_NEXT(h, ret), ++i) {
        if (h->nlmsg_type == NLMSG_ERROR) {
            return -1;
        }
        printf("Event received from Kernel: %s\n",(char *)NLA_DATA(((struct nlattr *) GENLMSG_DATA(h))));
    }

    return i;
}

int main(void) {
    int return_code;
//Step 1: Open & Bind the socket. Note that protocol = NETLINK_GENERIC
    return_code = netlink_open(&nl_fd, &nl_sequence_number, NETLINK_GENERIC,0);
    if (return_code < 0) {
        fprintf(stderr, "Error: Socket could not be created\n");
  return -1;
    }
    printf("%s() | Socket Opened\n", __func__);

//Step 2. Resolve the family ID corresponding to the string "EXMPL_EVENT"
    {
        char buffer[256];
        struct nlmsghdr *nlmsg = (struct nlmsghdr *)&buffer;
        struct nlattr *attrs[CTRL_ATTR_MAX + 1];

        nlmsg->nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
        nlmsg->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
        nlmsg->nlmsg_type = GENL_ID_CTRL;
        nlmsg->nlmsg_pid = getpid();

        struct genlmsghdr *ghdr = NLMSG_DATA(nlmsg);
        ghdr->cmd = CTRL_CMD_GETFAMILY;

        netlink_attr_attach(nlmsg, 128, CTRL_ATTR_FAMILY_NAME, nl_family_name, strlen(nl_family_name) + 1);

        if (netlink_send(nl_fd, &nl_sequence_number, nlmsg, 0, 0) < 0) {
            fprintf(stderr, "%s() | Error: sending family ID request message\n", __func__);
            netlink_close(nl_fd);
            return -1;
        }
        printf("%s() | Family Request Sent\n", __func__);

        nlmsg->nlmsg_len = 256;
        //Wait for the response message
        if (netlink_wait(nl_fd, &nl_sequence_number, nlmsg, 0) < 0) {
            fprintf(stderr, "Error: receiving family ID request message\n", __func__);
            netlink_close(nl_fd);
            return -1;
        }

        //Validate response message
        if (nlmsg->nlmsg_type != GENL_ID_CTRL) {
            fprintf(stderr, "%s() | Error: family ID request - invalid message\n", __func__);
            netlink_close(nl_fd);
            return -1;
        }
        ghdr = NLMSG_DATA(nlmsg);
        if (ghdr->cmd != CTRL_CMD_NEWFAMILY) {
            fprintf(stderr, "%s() | Error: family ID request - invalid message\n", __func__);
            netlink_close(nl_fd);
            return -1;
        }
        if (nlmsg->nlmsg_len < NLMSG_LENGTH(GENL_HDRLEN)){
            fprintf(stderr, "%s() | Error: family ID request - invalid message\n", __func__);
            netlink_close(nl_fd);
            return -1;
        }


        netlink_attr_parse(attrs, CTRL_ATTR_MAX, NLMSG_DATA(nlmsg) + GENL_HDRLEN, NLMSG_PAYLOAD(nlmsg, GENL_HDRLEN));

        if (attrs[CTRL_ATTR_FAMILY_ID]) {
            nl_family_id = *(__u32 *) (NLA_DATA(attrs[CTRL_ATTR_FAMILY_ID]));
            printf("%s() | Family ID resolved for \"%s\": %d\n", __func__, nl_family_name, nl_family_id);
        }

        if (attrs[CTRL_ATTR_MCAST_GROUPS]) {
            int i;
            struct nlattr *attrs2[GENL_MAX_FAM_GRPS + 1];
            netlink_attr_parse(attrs2, GENL_MAX_FAM_GRPS, NLA_DATA(attrs[CTRL_ATTR_MCAST_GROUPS]), attrs[CTRL_ATTR_MCAST_GROUPS]->nla_len - NLA_HDRLEN);

            for (i = 0; i < GENL_MAX_FAM_GRPS; i++) {
                if (netlink_verify_group(attrs2[i], &nl_group_id, nl_group_name) == 0) {
                    printf("%s() | Group ID resolved for \"%s\": %d\n", __func__, nl_group_name, nl_group_id);
                }
            }
        }

//Step 3. Close and Reopen Socket for specified group
        netlink_close(nl_fd);
        if (netlink_open(&nl_fd, &nl_sequence_number, NETLINK_GENERIC, nl_group_id ? (1 << (nl_group_id - 1)) : 0)) {
            fprintf(stderr, "Error: Socket could not be re-created\n");
            return -1;
        }

        netlink_wait_for_event(nl_fd, NULL, 10);
    }
    netlink_close(nl_fd);
    printf("%s() | Socket Closed\n",__func__);
    return 0;
}