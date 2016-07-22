#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<errno.h>

#define CHARS   80

static char line[CHARS];

static char nest[] = "     ";

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
int main()
{
	FILE *fip,*fop;
        fip = fopen("inp.txt", "r");
	fop = fopen("op.txt", "w+");
	char *name, *title, *company = NULL;
	int name_len, comp_len;
	while (fgets(line, sizeof(line), fip))
	{
		if (!strncmp("hello", line, strlen("hello")))
		{
			int len = strlen("hello");
			char *maker = gen_skip_space(&line[len]);
			name = maker;
			maker = gen_reach_space(maker);
			name_len = maker - name;
			//int end = maker  -line;
			maker = gen_skip_space(maker);
			title = maker;
			maker = gen_reach_space(maker);
			comp_len = maker - title;

			printf("%s",name);
			printf("%s",title);
			gen_write(fop, 0, "%s \"\n");
			gen_raw_write(fop, 0, name, name_len);
		}
	}
	fclose(fip);
	fclose(fop);
	return 0;
}
