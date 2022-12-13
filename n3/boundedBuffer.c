#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 20
#define NUMITEMS 30
#define NUM_PRODUCER 3
#define NUM_CONSUMER 3

// 버퍼 구조체
typedef struct
{
	int item[BUFFER_SIZE];
	int totalitems;
	int in, out;
	pthread_mutex_t mutex;
	pthread_cond_t full;
	pthread_cond_t empty;
} buffer_t;

buffer_t bb = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 
	PTHREAD_MUTEX_INITIALIZER, 
	PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER
};

// 아이템 생산
int produce_item(void *arg)
{
	int item = (int)(100.0 * rand() / (RAND_MAX + 1.0));
	sleep((unsigned long)(5.0 * rand() / (RAND_MAX + 1.0)));
	printf("producer[%d] : produce item = %d\n", arg, item);
	return item;
}

// 버퍼에 아이템 삽입
insert_item(int item)
{
	int status;
	status = pthread_mutex_lock(&bb.mutex);
	if (status != 0)
		return status;

	while (bb.totalitems >= BUFFER_SIZE && status == NULL)
		status = pthread_cond_wait(&bb.empty, &bb.mutex);
	if (status != 0)
	{
		pthread_mutex_unlock(&bb.mutex);
		return status;
	}

	bb.item[bb.in] = item;
	bb.in = (bb.in + 1) % BUFFER_SIZE;
	bb.totalitems++;
	if (status = pthread_cond_signal(&bb.full))
	{
		pthread_mutex_unlock(&bb.mutex);
		return status;
	}
	return pthread_mutex_unlock(&bb.mutex);
}

// 아이템 소비
consume_item(int item, void *arg)
{
	sleep((unsigned long)(5.0 * rand() / (RAND_MAX + 1.0)));
	printf("\t\tconsumer[%d] : consume item = %d\n", arg, item);
}

// 버퍼에서 아이템 가져오기
remove_item(int *temp)
{
	int status;
	status = pthread_mutex_lock(&bb.mutex);
	if (status != 0)
		return status;

	while (bb.totalitems <= 0 && status == NULL)
		status = pthread_cond_wait(&bb.full, &bb.mutex);
	if (status != 0)
	{
		pthread_mutex_unlock(&bb.mutex);
		return status;
	}

	*temp = bb.item[bb.out];
	bb.out = (bb.out + 1) % BUFFER_SIZE;
	bb.totalitems--;
	if (status = pthread_cond_signal(&bb.empty))
	{
		pthread_mutex_unlock(&bb.mutex);
		return status;
	}
	return pthread_mutex_unlock(&bb.mutex);
}

void *producer(void *arg)
{
	int item;
	while (1)
	{
		item = produce_item(arg);
		insert_item(item);
	}
}

void *consumer(void *arg)
{
	int item;
	while (1)
	{
		remove_item(&item);
		consume_item(item, arg);
	}
}

int main()
{
	int status;
	void *result;
	pthread_t producer_tid[NUM_PRODUCER], consumer_tid[NUM_CONSUMER];

	// 생산자 쓰레드 생성
	for (size_t i = 0; i < NUM_PRODUCER; i++)
	{
		status = pthread_create(&producer_tid[i], NULL, producer, (void *)i);
		if (status != 0)
		{
			perror("Create producer thread");
		}
	}
	// 소비자 쓰레드 생성
	for (size_t i = 0; i < NUM_CONSUMER; i++)
	{
		status = pthread_create(&consumer_tid, NULL, consumer, (void *)i);
		if (status != 0)
		{
			perror("Create consumer thread");
		}
	}
	// 생산자 쓰레드 결합
	for (size_t i = 0; i < NUM_PRODUCER; i++)
	{
		status = pthread_join(producer_tid, NULL);
		if (status != 0)
		{
			perror("Join producer thread");
		}
	}
	// 소비자 쓰레드 결합
	for (size_t i = 0; i < NUM_CONSUMER; i++)
	{
		status = pthread_join(consumer_tid, NULL);
		if (status != 0)
		{
			perror("Join consumer thread");
		}
	}
}
