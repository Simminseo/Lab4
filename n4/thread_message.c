#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_CLIENT 5

int hasRequest = 0;
char server_message[50]="sever message";

pthread_mutex_t client_mutex;
pthread_mutex_t server_mutex;
pthread_cond_t broad;
pthread_cond_t client_broad;

void *server(void *arg)
{
	while (1)
	{
		pthread_mutex_lock(&server_mutex);
		pthread_cond_wait(&broad, &server_mutex);
		pthread_cond_broadcast(&client_broad);
		hasRequest = 0;
		pthread_mutex_unlock(&server_mutex);
	}
}

void *client(void *arg)
{
	while (1)
	{
		if (hasRequest == 0)
		{
			hasRequest = 1;
			sleep(1);
			printf("request from client %d", (int)arg);
			printf("\n");
			pthread_cond_signal(&broad);
		}
		else
		{
			pthread_cond_wait(&client_broad, &client_mutex);
			printf("client %d Received : %s\n", arg, server_message);
		}
		pthread_mutex_unlock(&client_mutex);
	}
}

int main(int argc, char *argv[])
{
	int status;
	void *result;
	pthread_t server_tid, client_tid[NUM_CLIENT];

	pthread_mutex_init(&client_mutex, NULL);
	pthread_mutex_init(&server_mutex, NULL);
	pthread_cond_init(&broad, NULL);
	pthread_cond_init(&client_broad, NULL);

	status = pthread_create(&server_tid, NULL, server, NULL);
	if (status != 0)
	{
		perror("Create server thread");
	}

	for (int i = 0; i < NUM_CLIENT; i++)
	{
		status = pthread_create(&client_tid, NULL, client, i);
		if (status != 0)
		{
			perror("Create client thread");
		}
	}

	status = pthread_join(server_tid, NULL);
	if (status != 0)
		perror("Join server thread");

	for (int i = 0; i < NUM_CLIENT; i++)
	{
		status = pthread_join(client_tid, NULL);
		if (status != 0)
			perror("Join client thread");
	}

	return 0;
}
