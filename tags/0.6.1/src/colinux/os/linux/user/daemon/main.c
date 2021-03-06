/*
 * This source code is a part of coLinux source package.
 *
 * Dan Aloni <da-x@colinux.org>, 2003 (c)
 *
 * The code is licensed under the GPL. See the COPYING file at
 * the root directory.
 *
 */ 

#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <sched.h>

#include <colinux/user/daemon.h>
#include <colinux/user/monitor.h>
#include <colinux/user/manager.h>
#include <colinux/os/user/manager.h>
#include <colinux/os/user/misc.h>
#include <colinux/os/user/pipe.h>

static co_daemon_t *colinux_daemon = NULL;

void sighup_handler(int sig)
{
	co_terminal_print("Receieved SIGHUP\n");	

	colinux_daemon->send_ctrl_alt_del = PTRUE;
}

void set_affinity() 
{
#if (0)
	unsigned long set = 0;
	int ret;

	set = 1; /* Bind to the first CPU */
	sched_setaffinity(getpid(), &set, sizeof(set));
#endif
}

int main(int argc, char *argv[]) 
{
	co_rc_t rc = CO_RC_OK;
	co_start_parameters_t start_parameters;
	co_manager_handle_t handle;
	bool_t installed = PFALSE;
	int ret;

	co_terminal_print("Cooperative Linux daemon\n");

	rc = co_daemon_parse_args(argv, &start_parameters);
	if (!CO_OK(rc)) {
		co_terminal_print("daemon: error parsing parameters\n");
		co_daemon_syntax();
		return -1;
	}

	if (start_parameters.show_help) {
		co_daemon_syntax();
		return 0;
	}

	rc = co_os_manager_is_installed(&installed);
	if (!CO_OK(rc)) {
		co_terminal_print("daemon: error, unable to determine if driver is installed (rc %d)\n", rc);
		return -1;
	}
	
	if (!installed) {
		co_terminal_print("daemon: error, kernel module is not loaded\n");
		return -1;
	}

	handle = co_os_manager_open();
	if (handle) {
		co_manager_ioctl_status_t status = {0, };

		rc = co_manager_status(handle, &status);
		if (CO_OK(rc)) {
			co_terminal_print("daemon: manager is loaded\n");
			if (status.state == CO_MANAGER_STATE_NOT_INITIALIZED) {
    				co_terminal_print("daemon: initializing manager\n");
				rc = co_manager_init(handle, PTRUE);
				if (!CO_OK(rc)) {
					co_terminal_print("daemon: error initializing driver\n", rc);
					return -1;
				}	
    			}
		} else {
			co_terminal_print("daemon: can't get manager status\n");
			return -1;
		}
	} else {
		co_terminal_print("daemon: cannot open driver\n");
		return -1;
	}
	co_os_manager_close(handle);

	set_affinity();

	rc = co_daemon_create(&start_parameters, &colinux_daemon);
	if (!CO_OK(rc))
		goto out;

	rc = co_daemon_start_monitor(colinux_daemon);
	if (!CO_OK(rc))
		goto out_destroy;

	signal(SIGHUP, sighup_handler);
	rc = co_daemon_run(colinux_daemon);
	signal(SIGHUP, SIG_DFL);

	co_daemon_end_monitor(colinux_daemon);

out_destroy:
	co_daemon_destroy(colinux_daemon);
	colinux_daemon = NULL;

out:
	if (!CO_OK(rc)) {
                if (CO_RC_GET_CODE(rc) == CO_RC_OUT_OF_PAGES) {
			co_terminal_print("daemon: not enough physical memory available (try with a lower setting)\n", rc);
		} else 
			co_terminal_print("daemon: exit code: %x\n", rc);
		ret = -1;
	} else {
		ret = 0;
	}

	return ret;
}
