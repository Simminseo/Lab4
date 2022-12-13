#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MYPORT 3490
#define BUF_SIZE 100

pthread_t thread[10];
int client_fd_arr[10] = {0};

void *chat_thread(void *num)
{
	int i = *(int *)num;
	int k;
	char buf[BUF_SIZE];
	int ret;
	while (1)
	{
		if ((ret = read(client_fd_arr[i], buf, 1024)) < 0)
		{
			printf("read error : %d\n", ret);
			exit(-1);
		}
		else
		{
			if (ret == 0)
			{
				client_fd_arr[i] = 0;
				pthread_exit((void *)0);
			}
			else
			{
				for (k = 0; k < 10; k++)
				{
					if (client_fd_arr[k] != 0)
					{
						if ((write(client_fd_arr[k], buf, ret)) < 0)
						{
							printf("write error\n");
							exit(-1);
						}
					}
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	int server_fd;
	int client_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int sin_size;

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket error\n");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(MYPORT);

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind error");
		exit(1);
	}

	if (listen(server_fd, 10) == -1)
	{
		perror("listen error");
		exit(1);
	}

	while (1)
	{
		sin_size = sizeof(struct sockaddr_in);
		if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_size)) == -1)
		{
			perror("accept fail");
			continue;
		}
		else
		{
			for (int i = 0; i < 10; i++)
			{
				if (client_fd_arr[i] == 0)
				{
					client_fd_arr[i] = client_fd;
					pthread_create(&thread[i], NULL, &chat_thread, &i);
					break;
				}
			}
		}
	}
}
