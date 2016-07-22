/* attributes (variables):
 * the index in this enum is used as a reference for the type,
 * userspace application has to indicate the corresponding type
 * the policy is used for security considerations 
 */
enum {
 GENL_EXMPL_EVENT_A_UNSPEC,
 GENL_EXMPL_EVENT_A_MSG,
    __GENL_EXMPL_EVENT_A_MAX,
};
#define GENL_EXMPL_EVENT_A_MAX (__GENL_EXMPL_EVENT_A_MAX - 1)

/* commands: enumeration of all commands (functions), 
 * used by userspace application to identify command to be executed
 */
enum {
 GENL_EXMPL_EVENT_C_UNSPEC,
 GENL_EXMPL_EVENT_C_DO,
 __GENL_EXMPL_EVENT_C_MAX,
};
#define GENL_EXMPL_EVENT_C_MAX (__GENL_EXMPL_EVENT_C_MAX - 1)

#define GENL_EXMPL_EVENT_VERSION 1
#define GENL_EXMPL_EVENT_FAMILYNAME "CONTROL_EXMPL"
#define GENL_EXMPL_EVENT_GROUPNAME "EXMPL_GRP"

uint8_t genl_exmpl_event_init(void);
void genl_exmpl_event_deinit(void);
int genl_exmpl_event_send(u8 *msg, int len);
