 /*
 * Copyright (C) 2014-2017 Firejail Authors
 *
 * This file is part of firejail project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "../include/common.h"

#define MAXBUF 4098
#define MAXARGS 16
static char *args[MAXARGS] = {0};
static int argcnt = 0;
int arg_quiet = 0;


static char *default_filter =
"*filter\n"
":INPUT DROP [0:0]\n"
":FORWARD DROP [0:0]\n"
":OUTPUT ACCEPT [0:0]\n"
"-A INPUT -i lo -j ACCEPT\n"
"-A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT\n"
"# echo replay is handled by -m state RELATED/ESTABLISHED above\n"
"#-A INPUT -p icmp --icmp-type echo-reply -j ACCEPT\n"
"-A INPUT -p icmp --icmp-type destination-unreachable -j ACCEPT\n"
"-A INPUT -p icmp --icmp-type time-exceeded -j ACCEPT\n"
"-A INPUT -p icmp --icmp-type echo-request -j ACCEPT \n"
"# disable STUN\n"
"-A OUTPUT -p udp --dport 3478 -j DROP\n"
"-A OUTPUT -p udp --dport 3479 -j DROP\n"
"-A OUTPUT -p tcp --dport 3478 -j DROP\n"
"-A OUTPUT -p tcp --dport 3479 -j DROP\n"
"COMMIT\n";

static void usage(void) {
	printf("Usage:\n");
	printf("\tfnetfilter netfilter-command destination-file\n");
}


static void copy(const char *src, const char *dest) {
	FILE *fp1 = fopen(src, "r");
	if (!fp1) {
		fprintf(stderr, "Error fnetfilter: cannot open %s\n", src);
		exit(1);
	}

	FILE *fp2 = fopen(dest, "w");
	if (!fp2) {
		fprintf(stderr, "Error fnetfilter: cannot open %s\n", dest);
		exit(1);
	}

	char buf[MAXBUF];
	while (fgets(buf, MAXBUF, fp1))
		fprintf(fp2, "%s", buf);

	fclose(fp1);
	fclose(fp2);
}

static void process_template(char *src, const char *dest) {
	char *arg_start = strchr(src, ',');
	assert(arg_start);
	*arg_start = '\0';
	arg_start++;
	if (*arg_start == '\0') {
		fprintf(stderr, "Error fnetfilter: you need to provide at least on argument\n");
		exit(1);
	}

	// extract the arguments from command line
	char *token = strtok(arg_start, ",");
	while (token) {
		// look for abnormal things
		int len = strlen(token);
		if (strcspn(token, "\\&!?\"'<>%^(){};,*[]") != (size_t)len) {
			fprintf(stderr, "Error fnetfilter: invalid argument in netfilter command\n");
			exit(1);
		}
		args[argcnt] = token;
		argcnt++;
		token = strtok(NULL, ",");
	}
#if 0
{
printf("argcnt %d\n", argcnt);
int i;
for (i = 0; i < argcnt; i++)
	printf("%s\n", args[i]);
}
#endif

	// open the files
	FILE *fp1 = fopen(src, "r");
	if (!fp1) {
		fprintf(stderr, "Error fnetfilter: cannot open %s\n", src);
		exit(1);
	}

	FILE *fp2 = fopen(dest, "w");
	if (!fp2) {
		fprintf(stderr, "Error fnetfilter: cannot open %s\n", dest);
		exit(1);
	}

	int line = 0;
	char buf[MAXBUF];
	while (fgets(buf, MAXBUF, fp1)) {
		line++;
		char *ptr = buf;
		while (*ptr != '\0') {
			if (*ptr != '$')
				fputc(*ptr, fp2);
			else {
				// parsing
				int index = 0;
				int rv = sscanf(ptr, "$ARG%u", &index) ;
				if (rv != 1) {
					fprintf(stderr, "Error fnetfilter: invalid template argument on line %d\n", line);
					exit(1);
				}

				// print argument
				if (index < 1 || index > argcnt) {
					fprintf(stderr, "Error fnetfilter: $ARG%d on line %d was not defined\n", index, line);
					exit(1);
				}
				fprintf(fp2, "%s", args[index - 1]);

				// march to the end of argument
				ptr += 4;
				while (isdigit(*ptr))
					ptr++;
				ptr--;
			}
			ptr++;
		}
	}

	fclose(fp1);
	fclose(fp2);
}

int main(int argc, char **argv) {
#if 0
{
system("cat /proc/self/status");
int i;
for (i = 0; i < argc; i++)
	printf("*%s* ", argv[i]);
printf("\n");
}
#endif

	char *quiet = getenv("FIREJAIL_QUIET");
	if (quiet && strcmp(quiet, "yes") == 0)
		arg_quiet = 1;

	if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-?") ==0)) {
		usage();
		return 0;
	}

	if (argc != 2 && argc != 3) {
		usage();
		return 1;
	}

	char *destfile = (argc == 3)? argv[2]: argv[1];
	char *command = (argc == 3)? argv[1]: NULL;
//printf("command %s\n", command);
//printf("destfile %s\n", destfile);
	// destfile is a real filename
	int len = strlen(destfile);
	if (strcspn(destfile, "\\&!?\"'<>%^(){};,*[]") != (size_t)len) {
		fprintf(stderr, "Error fnetfilter: invalid destination file in netfilter command\n");
		exit(1);
	}

	// handle default config (command = NULL, destfile)
	if (command == NULL) {
		// create a default filter file
		FILE *fp = fopen(destfile, "w");
		if (!fp) {
			fprintf(stderr, "Error fnetfilter: cannot open %s\n", destfile);
			exit(1);
		}
		fprintf(fp, "%s\n", default_filter);
		fclose(fp);
	}
	else {
		if (strrchr(command, ','))
			process_template(command, destfile);
		else
			copy(command, destfile);
	}

	return 0;
}
