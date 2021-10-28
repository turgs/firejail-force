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

#include "fbuilder.h"
#include <sys/wait.h>
#include <fcntl.h>

#define TRACE_OUTPUT "/tmp/firejail-trace"
#define STRACE_OUTPUT "/tmp/firejail-strace"

static char *cmdlist[] = {
	"/usr/bin/firejail",
	"--quiet",
	"--output=" TRACE_OUTPUT,
	"--noprofile",
	"--caps.drop=all",
	"--nonewprivs",
	"--trace",
	"--shell=none",
	"/usr/bin/strace", // also used as a marker in build_profile()
	"-c",
	"-f",
	"-o" STRACE_OUTPUT,
};

static void clear_tmp_files(void) {
	unlink(STRACE_OUTPUT);
	unlink(TRACE_OUTPUT);

	// run all the rest
	int i;
	for (i = 1; i <= 5; i++) {
		char *newname;
		if (asprintf(&newname, "%s.%d", TRACE_OUTPUT, i) == -1)
			errExit("asprintf");
		unlink(newname);
		free(newname);
	}

}

void build_profile(int argc, char **argv, int index, FILE *fp) {
	// next index is the application name
	if (index >= argc) {
		fprintf(stderr, "Error: application name missing\n");
		exit(1);
	}

	// clean /tmp files
	clear_tmp_files();

	// detect strace
	int have_strace = 0;
	if (access("/usr/bin/strace", X_OK) == 0)
		have_strace = 1;

	// calculate command length
	unsigned len = (int) sizeof(cmdlist) / sizeof(char*) + argc - index + 1;
	if (arg_debug)
		printf("command len %d + %d + 1\n", (int) (sizeof(cmdlist) / sizeof(char*)), argc - index);
	char *cmd[len];
	cmd[0] = cmdlist[0];	// explicit assignemnt to clean scan-build error

	// build command
	unsigned i = 0;
	for (i = 0; i < (int) sizeof(cmdlist) / sizeof(char*); i++) {
		// skip strace if not installed
		if (have_strace == 0 && strcmp(cmdlist[i], "/usr/bin/strace") == 0)
			break;
		cmd[i] = cmdlist[i];
	}

	int i2 = index;
	for (; i < (len - 1); i++, i2++)
		cmd[i] = argv[i2];
	assert(i < len);
	cmd[i] = NULL;

	if (arg_debug) {
		for (i = 0; i < len; i++)
			printf("\t%s\n", cmd[i]);
	}

	// fork and execute
	pid_t child = fork();
	if (child == -1)
		errExit("fork");
	if (child == 0) {
		assert(cmd[0]);
		int rv = execvp(cmd[0], cmd);
		(void) rv;
		errExit("execv");
	}

	// wait for all processes to finish
	int status;
	if (waitpid(child, &status, 0) != child)
		errExit("waitpid");

	if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
		printf("\n\n\n");
		fprintf(fp, "############################################\n");
		fprintf(fp, "# %s profile\n", argv[index]);
		fprintf(fp, "############################################\n");
		fprintf(fp, "# Persistent global definitions\n");
		fprintf(fp, "# include /etc/firejail/globals.local\n");
		fprintf(fp, "\n");

		fprintf(fp, "### basic blacklisting\n");
		fprintf(fp, "include /etc/firejail/disable-common.inc\n");
		fprintf(fp, "# include /etc/firejail/disable-devel.inc\n");
		fprintf(fp, "include /etc/firejail/disable-passwdmgr.inc\n");
		fprintf(fp, "# include /etc/firejail/disable-programs.inc\n");
		fprintf(fp, "\n");

		fprintf(fp, "### home directory whitelisting\n");
		build_home(TRACE_OUTPUT, fp);
		fprintf(fp, "\n");

		fprintf(fp, "### filesystem\n");
		build_tmp(TRACE_OUTPUT, fp);
		build_dev(TRACE_OUTPUT, fp);
		build_etc(TRACE_OUTPUT, fp);
		build_var(TRACE_OUTPUT, fp);
		build_bin(TRACE_OUTPUT, fp);
		build_share(TRACE_OUTPUT, fp);
		fprintf(fp, "\n");

		fprintf(fp, "### security filters\n");
		fprintf(fp, "caps.drop all\n");
		fprintf(fp, "nonewprivs\n");
		fprintf(fp, "seccomp\n");
		if (have_strace)
			build_seccomp(STRACE_OUTPUT, fp);
		else {
			fprintf(fp, "# If you install strace on your system, Firejail will also create a\n");
			fprintf(fp, "# whitelisted seccomp filter.\n");
		}
		fprintf(fp, "\n");

		fprintf(fp, "### network\n");
		build_protocol(TRACE_OUTPUT, fp);
		fprintf(fp, "\n");

		fprintf(fp, "### environment\n");
		fprintf(fp, "shell none\n");

	}
	else {
		fprintf(stderr, "Error: cannot run the sandbox\n");
		exit(1);
	}
}
