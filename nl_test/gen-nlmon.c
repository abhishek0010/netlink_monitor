#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>

#define CHARS   80

static char line[CHARS];
static char nest[] = "    ";

static char header[] = {"\
/*\n\
 * Auto generated file\n\
 * nlmon.c -- Utility for monitoring netlink communication of vRouter-Agent\n\
 */\n"
};

static char includes[] = {"\
#include <stdio.h>\n\
#include <stdlib.h>\n\
#include <unistd.h>\n\
#include <string.h>\n\
#include <errno.h>\n\
#include <getopt.h>\n\
#include <stdbool.h>\n\
#include <inttypes.h>\n\
#include <signal.h>\n\
\
#include <sys/types.h>\n\n\
#include <net/if.h>\n\n\
\
#include <sys/socket.h>\n\
#if defined(__linux__)\n\
#include <linux/netlink.h>\n\
#include <linux/rtnetlink.h>\n\
#include <linux/if_ether.h>\n\
#elif defined(__FreeBSD__)\n\
#include <net/ethernet.h>\n\
#endif\n\
\
#include <linux/genetlink.h>\n\n\
\
//#include <libxml/xmlmemory.h>\n\
//#include <libxml/parser.h>\n\n\
\
#include \"vr_genetlink.h\"\n\
#include \"nl_util.h\"\n\n\
#include \"ini_parser.h\"\n\
#include \"vr_types.h\"\n\n\
#include \"vr_nexthop.h\"\n\
\
//#include <vr_types.h>\n\
//#include <vt_gen_lib.h>\n\
//#include <vtest.h>\n\n\
\
#define GROUP_ID 31\n"
};

static char create_nl_client[] = {"\
static struct nl_client *cl;\n\n\
static bool reader = true;\n"
};

static char create_main[] = {"\
void reader_nl()\n\
{\n\
    int ret;\n\
    ret = nlmon_recvmsg_decoded(cl,true);\n\
    if (ret < 0)\n\
        printf(\"ret < 0.\\n\");\n\
    else\n\
    {\n\
        printf(\"ret = %d\\n\", ret);\n\
    }\n\
}\n\
\
void signal_handler(int dummy)\n\
{\n\
    reader = false;\n\
    nl_free_client(cl);\n\
    printf(\"\\nInterrupted Signal\\nClient freed.\\n\");\n\
    signal(SIGINT, signal_handler);\n\
}\n\
\
int main(int argc, char *argv[]) {\n\
\
    int group = GROUP_ID;\n\n\
    int opt, option_index;\n\
\
    signal(SIGINT, signal_handler);\n\
\
\
    cl = nlmon_connect(group);\n\
    printf(\"connected..\\n\");\n\
    while(reader)\n\
    {\n\
        reader_nl();\n\
    }\n\
    return 0;\n\
}\n"
};


#define GENLMSG_DATA(glh)       ((void *)(NLMSG_DATA(glh) + GENL_HDRLEN))
#define NLA_DATA(na)            ((void *)((char *)(na) + NLA_HDRLEN))

static unsigned char *
gen_skip_space(unsigned char *string)
{
    unsigned int i = 0, len;

    if (!string)
        return string;

    len = strlen(string);
    if (!len)
        return NULL;

    while ((i < len) && isspace(string[i])) {
        i++;
    }

    if (i == len)
        return NULL;

    return &string[i];
}

static unsigned char *
gen_reach_char(unsigned char *string, unsigned char c)
{
    unsigned int i = 0, len;

    if (!string)
        return string;

    len = strlen(string);
    if (!len)
        return string;

    while ((i < len) && (string[i] != c)) {
        i++;
    }

    return &string[i];
}

static unsigned char *
gen_reach_space(unsigned char *string)
{
    return gen_reach_char(string, ' ');
}


static void
gen_write(FILE *ofp, unsigned int nesting, unsigned char *string)
{
    unsigned int i;

    for (i = 0; i < nesting; i++)
        fwrite(nest, 1, strlen(nest), ofp);

    fwrite(string, 1, strlen(string), ofp);
    return;
}

static void
gen_raw_write(FILE *ofp, unsigned int nesting,
        unsigned char *string, unsigned int len)
{
    unsigned int i;

    for (i = 0; i < nesting; i++)
        fwrite(nest, 1, strlen(nest), ofp);

    fwrite(string, 1, len, ofp);
    return;
}

static void
gen_close(FILE *fp, bool header_file)
{
    if (header_file) {
        gen_write(fp, 0, "\n#endif\n");
    }

    fclose(fp);
    return;
}

static FILE *
gen_open(unsigned char *name, bool header_file)
{
    unsigned char c[105];
    unsigned int i = 0, j = 0;
    FILE *fp;

    if (header_file && (strlen(name) >= 100)) {
        perror(name);
        return NULL;
    }

    fp = fopen(name, "w+");
    if (!fp)
        return fp;

    gen_write(fp, 0, header);
//not needed
    if (header_file) {
        gen_write(fp, 0, "#ifndef ");
        c[i++] = '_';
        c[i++] = '_';
        while ((c[i] = name[j]) != '\0') {
            if (c[i] == '.') {
                c[i] = '_';
            } else {
                c[i] = toupper(c[i]);
            }
            i++, j++;
        }
        c[i++] = '_';
        c[i++] = '_';

        c[i] = '\0';

        gen_write(fp, 0, c);
        gen_write(fp, 0, "\n");
        gen_write(fp, 0, "#define ");
        gen_write(fp, 0, c);
        gen_write(fp, 0, "\n\n");
    }

    gen_write(fp, 0, includes);

    return fp;
}

static int
gen(FILE *fp)
{
	bool need_else = false;
	unsigned int len, start = 0, end = 0;
    unsigned int type_len, sub_type_len = 0, var_len;
    unsigned int nesting = 0;

    char *marker, *type, *var, *sub_type = NULL;

    FILE *nlmon;

    nlmon = gen_open("nlmonv2.c", false);
    if (!nlmon) {
        perror("nlmonv2.c");
        return errno;
    }

    gen_write(nlmon, 0, create_nl_client);

    while (fgets(line, sizeof(line), fp)) {
    	if (!strncmp("buffer sandesh", line, strlen("buffer sandesh"))) {
            nesting = 0;
            len = strlen("buffer sandesh");
            marker = gen_skip_space(&line[len]);
            if (!marker)
                break;
            //for get type of response
            start = marker - line;
            marker = gen_reach_space(marker);
            end = marker - line;
            gen_write(nlmon, nesting, "void\n");
            gen_raw_write(nlmon, nesting, &line[start], end - start);
            gen_write(nlmon, nesting, "_process(void *s)\n");
            gen_write(nlmon, nesting, "{\n");
            gen_raw_write(nlmon, ++nesting, &line[start], end - start);
            gen_write(nlmon, 0, " *req = (");
            //gen_write(nlmon, 0, " = ");
            //gen_write(nlmon, 0, " (");
            gen_raw_write(nlmon, 0, &line[start], end - start);
            gen_write(nlmon, 0, " *)s;\n");
            gen_write(nlmon, nesting, "printf(\\n");
            gen_write(nlmon, 0, "\"");
            gen_raw_write(nlmon, 0, &line[start], end - start);
            gen_write(nlmon, 0, " received :\\n");
            gen_write(nlmon, 0, "\"");
            gen_write(nlmon, 0, ");\n");
            gen_write(nlmon, nesting, "int i = 0;\n");

            /*while (fgets(line, sizeof(line), fp) || (strncmp("}", line, strlen("}"))!=0)) {
            	char *m1, *m2 = NULL;
            	char type[CHARS], var[CHARS];
            	m1 = gen_skip_space(&line[0]);
            	if (!m1)
                	break;
                m2 = gen_reach_space(m1);
                m1 = gen_skip_space(&line[m2]);
                if (!m1)
                	break;

                m2 = gen_reach_space(m1);
            }*/

            continue;
    	}
    	if (start) {
    		if (!strncmp("}", line, strlen("}"))) {
    			//modify here
    			start = end = 0;
    			gen_write(nlmon, 0, "\n");
    			gen_write(nlmon, 1, "return;\n");
    			gen_write(nlmon, 0, "}\n");
    			continue;
    		}
    		            marker = strchr(line, ':');
            if (!marker) {
                return EINVAL;
            }

            marker = gen_skip_space(++marker);
            if (!marker) {
                return EINVAL;
            }

            type = marker;
            marker = gen_reach_space(marker);
            if (!marker) {
                return EINVAL;
            }
            type_len = marker - type;

            if (!strncmp(type, "list", strlen("list"))) {
                marker = type + strlen("list");;
                marker = gen_skip_space(marker);
                if (!marker) {
                    return EINVAL;
                }

                if (*marker != '<') {
                    return EINVAL;
                }

                sub_type = ++marker;
                marker = gen_reach_char(marker, '>');
                if (!marker) {
                    return EINVAL;
                }
                sub_type_len = marker - sub_type;
                marker = gen_reach_space(marker);
                if (!marker) {
                    return EINVAL;
                }
            }

            marker = gen_skip_space(marker);
            if (!marker) {
                return EINVAL;
            }

            var = marker;
            marker = gen_reach_char(marker, ';');
            if (!marker) {
                return EINVAL;
            }
            var_len = marker - var;

            //skipped need_else

            if (!strncmp(type, "i32", strlen("i32"))) {
            	gen_write(nlmon, nesting, "printf(\"");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, " : %\" PRId32 \"\\n\", req->");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, ");\n");
            } 
            else if (!strncmp(type, "u32", strlen("u32"))) {
                gen_write(nlmon, nesting, "printf(\"");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, " : %\" PRIu32 \"\\n\", req->");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, ");\n");
            } 
            else if (!strncmp(type, "i64", strlen("i64"))) {
                gen_write(nlmon, nesting, "printf(\"");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, " : %\" PRId64 \"\\n\", req->");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, ");\n");
            } 
            /*else if ((!strncmp(type, "i16", strlen("i16"))) ||
                    (!strncmp(type, "u16", strlen("u16")))) {
                gen_write(ofp, 0, "strtoul(node->children->content, NULL, 0);\n");

                gen_write(fp_expect, 0, "result = vt_gen_short_compare(");
                gen_write(fp_expect, 0, "req->");
                gen_raw_write(fp_expect, 0, var, var_len);
                gen_write(fp_expect, 0, ",\n");
                gen_write(fp_expect, nesting + 4, "strtoul(node->children->content, NULL, 0));\n");

            } */
            else if (!strncmp(type, "byte", strlen("byte"))) {
                gen_write(nlmon, nesting, "printf(\"");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, " : %d\\n\", req->");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, ");\n");

            } 
            else if (!strncmp(type, "sandesh_op", strlen("sandesh_op"))) {
                gen_write(nlmon, nesting, "printf(\"");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, " : %d\\n\", req->");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, ");\n");

            } else if (!strncmp(type, "flow_op", strlen("flow_op"))) {
                gen_write(nlmon, nesting, "printf(\"");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, " : %d\\n\", req->");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, ");\n");

            } else if (!strncmp(type, "list", strlen("list"))) {
            	
                if (!strncmp(sub_type, "byte", strlen("byte"))) {
		    gen_write(nlmon, nesting, "printf(\"");
            	    gen_raw_write(nlmon, 0, var, var_len);
		    gen_write(nlmon, 0, " list values : \");\n");
		    gen_write(nlmon, nesting, "i = 0;\n");
		    gen_write(nlmon, nesting, "while(req->");
		    gen_raw_write(nlmon, 0, var, var_len);
		    gen_write(nlmon, 0, "[i])\n");
		    gen_write(nlmon, nesting, "{\n");
		    gen_write(nlmon, nesting + 1, "printf(\"%d  \", req->");
		    gen_raw_write(nlmon, 0, var, var_len);
		    gen_write(nlmon, 0, "[i]);\n");
		    gen_write(nlmon, nesting + 1, "i++;\n");
		    gen_write(nlmon, nesting, "}\n");
		    gen_write(nlmon, nesting, "printf(\"\\n\");\n");
                } else if (!strncmp(sub_type, "i16", strlen("i32"))) {
                    gen_write(nlmon, nesting, "printf(\"");
                    gen_raw_write(nlmon, 0, var, var_len);
                    gen_write(nlmon, 0, " list values : \");\n");
                    gen_write(nlmon, nesting, "i = 0;\n");
                    gen_write(nlmon, nesting, "while(req->");
                    gen_raw_write(nlmon, 0, var, var_len);
                    gen_write(nlmon, 0, "[i])\n");
                    gen_write(nlmon, nesting, "{\n");
                    gen_write(nlmon, nesting + 1, "printf(\"%\" PRId16 \"  \", req->");
                    gen_raw_write(nlmon, 0, var, var_len);
                    gen_write(nlmon, 0, "[i]);\n");
                    gen_write(nlmon, nesting + 1, "i++;\n");
                    gen_write(nlmon, nesting, "}\n");
                    gen_write(nlmon, nesting, "printf(\"\\n\");\n");
		} else if (!strncmp(sub_type, "i32", strlen("i32"))) {
                    gen_write(nlmon, nesting, "printf(\"");
                    gen_raw_write(nlmon, 0, var, var_len);
                    gen_write(nlmon, 0, " list values : \");\n");
                    gen_write(nlmon, nesting, "i = 0;\n");
                    gen_write(nlmon, nesting, "while(req->");
                    gen_raw_write(nlmon, 0, var, var_len);
                    gen_write(nlmon, 0, "[i])\n");
                    gen_write(nlmon, nesting, "{\n");
                    gen_write(nlmon, nesting + 1, "printf(\"%\" PRId32 \"  \", req->");
                    gen_raw_write(nlmon, 0, var, var_len);
                    gen_write(nlmon, 0, "[i]);\n");
                    gen_write(nlmon, nesting + 1, "i++;\n");
                    gen_write(nlmon, nesting, "}\n");
                    gen_write(nlmon, nesting, "printf(\"\\n\");\n");
		} else if (!strncmp(sub_type, "i64", strlen("i32"))) {
                    gen_write(nlmon, nesting, "printf(\"");
                    gen_raw_write(nlmon, 0, var, var_len);
                    gen_write(nlmon, 0, " list values : \");\n");
                    gen_write(nlmon, nesting, "i = 0;\n");
                    gen_write(nlmon, nesting, "while(req->");
                    gen_raw_write(nlmon, 0, var, var_len);
                    gen_write(nlmon, 0, "[i])\n");
                    gen_write(nlmon, nesting, "{\n");
                    gen_write(nlmon, nesting + 1, "printf(\"%\" PRId64 \"  \", req->");
                    gen_raw_write(nlmon, 0, var, var_len);
                    gen_write(nlmon, 0, "[i]);\n");
                    gen_write(nlmon, nesting + 1, "i++;\n");
                    gen_write(nlmon, nesting, "}\n");
                    gen_write(nlmon, nesting, "printf(\"\\n\");\n");
		}

   
            } 
            else if (!strncmp(type, "string", strlen("string"))) {
                gen_write(nlmon, nesting, "printf(\"");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, " : %s\", req->");
            	gen_raw_write(nlmon, 0, var, var_len);
            	gen_write(nlmon, 0, ");\n");
            }
        //}
		} 
	}
    gen_write(nlmon, 0, create_main);
}

Usage(void)
{
    printf("Usage: gen_nlmon <file name>\n");
    return;
}

int main(int argc, char *argv[])
{
	int ret;
    FILE *fp;

    if (argc != 2) {
        Usage();
        return EINVAL;
    }

    fp = fopen(argv[1], "r");
    if (!fp) {
        perror(argv[1]);
        return errno;
    }

    ret = gen(fp);
    return ret;

}
