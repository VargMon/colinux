/*
 * This source code is a part of coLinux source package.
 *
 * Dan Aloni <da-x@gmx.net>, 2003 (c)
 *
 * The code is licensed under the GPL. See the COPYING file at
 * the root directory.
 *
 */

#include "console.h"

static console_window_t *global_window = 0;

void
co_user_console_handle_scancode(co_scan_code_t sc)
{
	if (!global_window)
		return;

	global_window->handle_scancode(sc);
}

int
co_user_console_main(int argc, char **argv)
{
	co_rc_t rc;
	console_window_t window;

	global_window = &window;

	rc = window.parse_args(argc, argv);
	if (!CO_OK(rc)) {
		co_debug("The console program was unable to parse the parameters.\n");
		goto co_user_console_main_error;
	}

	rc = window.start();
	if (!CO_OK(rc)) {
		co_debug("The console program could not start.\n");
		goto co_user_console_main_error;
	}

	do {
		rc = window.loop();
	} while (CO_OK(rc) && window.online());

	window.online(false);

	if (CO_OK(rc))
		return 0;

      co_user_console_main_error:
	co_debug("The console program encountered an error: %08x\n", rc);
	return -1;
}
