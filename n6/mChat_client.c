#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUF_SIZE 100
#define IP "127.0.0.1"
#define PORT 3490

pthread_t thread;

void *chat_thread(void *my_fd)
{
    char buf[BUF_SIZE];
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        read(*(int *)my_fd, buf, BUF_SIZE);
        printf("message : %s\n", buf);
    }
}

int main(int argc, char *argv[])
{
    char buf[BUF_SIZE];
    struct sockaddr_in server_addr;
    int my_fd;
    int message_size;

    if ((my_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP);
    server_addr.sin_port = htons(PORT);

    if (connect(my_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect error");
        exit(1);
    }
    pthread_create(&thread, NULL, &chat_thread, &my_fd);

    while (1)
    {
        scanf("%s", buf);
        printf("my ");
        message_size = strlen(buf) + 1;
        if ((write(my_fd, buf, message_size)) == -1)
        {
            perror("write error");
            exit(1);
        }
    }
}
