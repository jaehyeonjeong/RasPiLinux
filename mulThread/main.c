#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_NUM 100
#define PLUS_NUM 10
#define FILE_PATH "pthread_debug.txt"

pthread_mutex_t share_memory_control;
int share_memory = 0;

void *count_share_memory(void *arg)
{
	int fd = 0;
	for(int i = 0; i < MAX_NUM; i++){
		pthread_mutex_lock(&share_memory_control);	//뮤텍스 잠금?
		fd = open((const char *)FILE_PATH, O_CREAT | O_APPEND | O_RDWR, 0644);
		if(fd == -1){
			printf("open() fail: %s\n", strerror(errno));
			exit(0);
		}
		//서식에 맞추어 파일로 출력하는 dprintf함수
		dprintf(fd, "[Child ID: %lu] share_memory: %d\n", pthread_self(), share_memory);
		close(fd);
		share_memory++;
		sleep(1);
		pthread_mutex_unlock(&share_memory_control);
	}
}

void *plus_share_memory(void *arg)
{
	int fd = 0;
	for(int i = 0; i < 10; i++){
		pthread_mutex_lock(&share_memory_control);
		fd = open((const char *)FILE_PATH, O_CREAT | O_APPEND | O_RDWR, 0644);
		if(fd == -1){
			printf("open() fail: %s\n", strerror(errno));
			exit(0);
		}
		dprintf(fd, "[Child ID: %lu] share_memory: %d\n", pthread_self(), share_memory);
		share_memory += (int)PLUS_NUM;
		close(fd);
		pthread_mutex_unlock(&share_memory_control);
		sleep(3);
	}
}

int main(int argc, char **argv)
{
	pthread_t pthread_list[2];
	int ret = 0;

	if(pthread_mutex_init(&share_memory_control, NULL)){
		printf("pthread_mutex_init fail: %s\n", strerror(errno));
		return -1;
	}
	
	printf("[MAIN] Thread id: %lu\n", pthread_self());

	ret = pthread_create(&pthread_list[0], NULL, count_share_memory, NULL);
	if(ret){
		printf("pthread_create fail: %s\n", strerror(errno));
		return -1;
	}
	ret = pthread_create(&pthread_list[1], NULL, plus_share_memory, NULL);
	if(ret){
		printf("pthread_create fail: %s\n", strerror(errno));
		return -1;
	}

	printf("trying to join 0: %lu\n", pthread_list[0]);
	if(pthread_join(pthread_list[0], NULL)){
		printf("pthread_join(%lu) fail\n", pthread_list[0]);
	}

	printf("trying to join 1: %lu\n", pthread_list[1]);
	if(pthread_join(pthread_list[1], NULL)){
		printf("pthread_join(%lu) fail\n", pthread_list[0]);
	}

	if(pthread_detach(pthread_list[0]) != 0){
		printf("pthread_detach 0 fail: %s\n", strerror(errno));
		return -1;
	}
	if(pthread_detach(pthread_list[1]) != 0){
		printf("pthread_detach 1 fail: %s\n", strerror(errno));
		return -1;
	}

	if(pthread_mutex_destroy(&share_memory_control) != 0){
		printf("pthread_mutex_destroy fail: %s\n", strerror(errno));
		return -1;
	}

	return 0;
}
