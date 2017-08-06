/** @file      main.c 
 *  @brief     POSIX信号量，未命名信号量，多线程之间的通信
 *  @note      
 *  @note      
 *  @author   zhangyinliang
 *  @date     2017.08.06
 *  @version   v1.0
*/
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//线程函数

sem_t sem;//信号量

#define MSG_SIZE 512

void* thread_send(void *msg)
{
	while(strcmp("end\n", msg) != 0)
	{
	    //输入信息，以输入end结束，由于fgets会把回车（\n）也读入，所以判断时就变成了“end\n”
	    printf("Input some text. Enter 'end'to finish...\n");
		fgets(msg, MSG_SIZE, stdin);
		//把信号量加1
		sem_post(&sem);
	}
	//退出线程
	pthread_exit(NULL);
}

void* thread_receive(void *msg)
{
	//把信号量减1
	sem_wait(&sem);
	char *ptr = msg;
	while(strcmp("end\n", msg) != 0)
	{
		int i = 0;
		//把小写字母变成大写
		for(; ptr[i] != '\0'; ++i)
		{
			if(ptr[i] >= 'a' && ptr[i] <= 'z')
			{
				ptr[i] -= 'a' - 'A';
			}
		}
		printf("You input %d characters\n", i-1);
		printf("To Uppercase: %s\n", ptr);
		//把信号量减1
		sem_wait(&sem);
	}
	//退出线程
	pthread_exit(NULL);
}

void init_task()
{
	int res = -1;
	pthread_t send_thread, receive_thread;
	void *thread_result = NULL;
	char msg[MSG_SIZE];
	//初始化信号量，其初值为0
	res = sem_init(&sem, 0, 0);
	if(res == -1)
	{
		perror("semaphore intitialization failed\n");
		exit(EXIT_FAILURE);
	}
	//创建发送线程，并把msg作为线程函数的参数
	res = pthread_create(&send_thread, NULL, thread_send, msg);
	if(res != 0)
	{
		perror("send pthread_create failed\n");
		exit(EXIT_FAILURE);
	}
	
    //创建接收线程，并把msg作为线程函数的参数
	res = pthread_create(&receive_thread, NULL, thread_receive, msg);
	if(res != 0)
	{
		perror("receive pthread_create failed\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Waiting for thread to finish...\n");
	
	//等待子线程结束
	res = pthread_join(send_thread, &thread_result);
	if(res != 0)
	{
		perror("send pthread_join failed\n");
		exit(EXIT_FAILURE);
	}
	res = pthread_join(receive_thread, &thread_result);
	if(res != 0)
	{
		perror("receive pthread_join failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Thread joined\n");
	//清理信号量
	sem_destroy(&sem);
	exit(EXIT_SUCCESS);
}

int main()
{
	init_task();
	exit(EXIT_SUCCESS);
}
