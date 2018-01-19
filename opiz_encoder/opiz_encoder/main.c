
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "opiz_gpio.h"
#include "stdio.h"
#include "rotaryencoder.h"
#include "main.h"

#if __STDC_NO_ATOMICS__ != 1
#include <stdatomic.h>
#endif



#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

static long valueA = 0;

void *encoder_thread_proc(void *tid)
{
	struct encoder *encoder = setupencoder(OPIZ_GPA(0), OPIZ_GPA(10));
	long value;
	while (1)
	{
		updateEncoders();
		long l = (encoder->value) / 4;
		if (l != value)
		{
			printf("value: %d\n", (void *)l);
			value = l;
		}
		valueA = value;
	}
	pthread_exit(NULL);
}

int main()
{
	printf("ATOMIC_INT_LOCK_FREE:%d\n", ATOMIC_INT_LOCK_FREE);
	printf("sizeof(long):%d\n", sizeof(long));
	opiz_gpio_init();
	create_encoder_thread();
	run_tcp_server();
	return 0;
}

void run_tcp_server()
{
	int sock, listener;
	struct sockaddr_in addr;
	char buf[1024];
	int bytes_read;
		
	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0)
	{
		perror("socket");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3425);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		exit(2);
	}

	listen(listener, 1);

	while (1)
	{
		sock = accept(listener, NULL, NULL);
		if (sock < 0)
		{
			perror("accept");
			exit(3);
		}

		while (1)
		{
			bytes_read = recv(sock, buf, 1024, 0);
			if (bytes_read <= 0) break;
			//if (strncmp(buf, "GETSTATE", MIN(bytes_read, 8) == 0))
			{
				snprintf(buf, 1024, "%d\0", valueA);
				send(sock, buf, strlen(buf) + 1, 0);
			}
		}

		close(sock);
	}
}

void create_encoder_thread()
{
	pthread_t encoder_thread;
	int status = pthread_create(&encoder_thread, NULL, encoder_thread_proc, NULL);
	if (status != 0)
	{
		printf("Error %d\n", status);
		exit(-1);
	}
}
