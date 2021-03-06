#include <stdio.h>
#include <string.h>

#include <colinux/os/alloc.h>
#include <colinux/os/user/pipe.h>
#include <colinux/os/user/daemon.h>

int client()
{
	co_rc_t rc;
	co_daemon_handle_t handle;
	struct {
		co_message_t message;
		char payload[0x2000]; 
	} message = {{0,}}, *in_message;
	int i = 0;

	rc = co_os_open_daemon_pipe(0, 41234, &handle);

	if (!CO_OK(rc)) {
		co_debug("co_os_open_daemon_pipe: %x\n", rc);
		return rc;
	}

	while (1) {
		/* usleep(100000); */
		i++;

		printf("sent %d\n", i);
		message.message.size = 0x2000;
		message.message.from = 0x10;
		snprintf(message.payload, sizeof(message.payload), "asd");
		co_os_daemon_send_message(handle, &message.message);

		printf("receiving %d\n", i);
		rc = co_os_daemon_get_message(handle, &in_message, 10);

		if (!CO_OK(rc)) {
			printf("error: %08x\n", rc);
		} else {
			if (in_message != NULL) {
				printf("got message\n");
				co_os_free(in_message);
			}
		}

	}

	co_os_daemon_close(handle);

	return 0;
}

typedef struct {
	long state;
} client_data_t;


co_rc_t connected(co_os_pipe_connection_t *conn,
		  void *data, 
		  void **data_client)
{
	client_data_t *client_data;

	printf("%p: %s\n", conn, __FUNCTION__);

	client_data = (client_data_t *)co_os_malloc(sizeof(client_data_t));
	client_data->state = 0;

	*data_client = (void *)client_data;
	
	return CO_RC(OK);
}

co_rc_t packet(co_os_pipe_connection_t *conn, 
	       void **data,
	       char *packet_data,
	       unsigned long size)
{
	client_data_t *client_data;

	client_data = (client_data_t *)*data;

	printf("%p: %s (%d)\n", conn, __FUNCTION__, size);

	if (client_data->state == 0) {
		if (size != sizeof(unsigned long)) {
			co_debug("Size of first packet is not unsigned long\n");
			return CO_RC(ERROR);
		}
		co_debug("id: %d\n", (*(unsigned long *)packet_data));
		client_data->state += 1;
	} else {
		int i;
		client_data->state++;

		printf("packet: sending back\n");
		co_os_pipe_server_send(conn, packet_data, size);
		printf("packet: sending back done\n");

#if (0)
		printf("packet: %d\n", client_data->state);
		for (i=0; i < size; i++) {
			co_debug("%02x ", packet_data[i]);
			if ((i % 16 == 0)  &&  (i != 0))
				co_debug("\n");
		}
		if (size % 16 != 0)
			co_debug("\n");
#endif
	}

	return CO_RC(OK);
}

void disconnected(co_os_pipe_connection_t *conn, void **data)
{
	printf("%p: %s\n", conn, __FUNCTION__);

	co_os_free(*data);
}

int server()
{
	co_os_pipe_server_t *ps;
	co_rc_t rc;
	
	rc = co_os_pipe_server_create(connected, packet, disconnected, NULL, &ps);
	if (!CO_OK(rc)) {
		co_debug("pipe_server_create: %x\n", rc);
		return rc;
	}

	while (1) {
		rc = co_os_pipe_server_service(ps, PTRUE);
	}

	co_os_pipe_server_destroy(ps);

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Invalid parmater\n");
		return 0;
	}
		
	if (strcmp(argv[1], "client") == 0) {
		return client();
	}

	return server();
}
