/*
 * Copyright (c) International Business Machines  Corp., 2001
 *	         written by Wayne Boyer
 * Copyright (c) 2013 Markos Chandras
 * Copyright (c) 2013 Cyril Hrubis <chrubis@suse.cz>
 *
 * This program is free software;  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY;  without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;  if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "test.h"
#include "usctest.h"
#include "getdents.h"

static void cleanup(void);
static void setup(void);

char *TCID = "getdents03";
int TST_TOTAL = 1;

static int exp_enos[] = { EINVAL, 0 };

static int longsyscall;

static option_t options[] = {
		/* -l long option. Tests getdents64 */
		{"l", &longsyscall, NULL},
		{NULL, NULL, NULL}
};

static void help(void)
{
	printf("  -l      Test the getdents64 system call\n");
}

int main(int ac, char **av)
{
	int lc;
	char *msg;
	int rval, fd;
	char buf[1];

	if ((msg = parse_opts(ac, av, options, &help)) != NULL)
		tst_brkm(TBROK, NULL, "OPTION PARSING ERROR - %s", msg);

	setup();

	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;

		if ((fd = open(".", O_RDONLY)) == -1)
			tst_brkm(TBROK, cleanup, "open of directory failed");

		/* Pass one byte long buffer. The result should be EINVAL */
		if (longsyscall)
			rval = getdents64(fd, (void *)buf, sizeof(buf));
		else
			rval = getdents(fd, (void *)buf, sizeof(buf));

		/*
		 * Hopefully we get an error due to the small buffer.
		 */
		if (rval < 0) {
			TEST_ERROR_LOG(errno);

			switch (errno) {
			case EINVAL:
				tst_resm(TPASS,
					 "getdents failed with EINVAL as expected");
			break;
			case ENOSYS:
				tst_resm(TCONF, "syscall not implemented");
			break;
			default:
				tst_resm(TFAIL | TERRNO,
					 "getdents call failed unexpectedly");
				break;
			}
		} else {
			tst_resm(TFAIL, "getdents passed unexpectedly");
		}

		if (close(fd) == -1)
			tst_brkm(TBROK, cleanup, "fd close failed");
	}

	cleanup();
	tst_exit();
}

static void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);

	TEST_PAUSE;

	tst_tmpdir();

	TEST_EXP_ENOS(exp_enos);

	TEST_PAUSE;
}

static void cleanup(void)
{
	TEST_CLEANUP;

	tst_rmdir();
}
