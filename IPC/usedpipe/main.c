#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#define MAX_BUF_LEN 1028

static void recv_data(int fd)
{
	char buf[MAX_BUF_LEN];
	memset((void *)buf, 0, sizeof(buf));
	if(read(fd, (void *)buf, sizeof(buf)) == -1){
		perror("read()");
		return ;
	}
	printf("[Recv Data] - [Child] pid: %u, %s\n", getpid(), buf);
}

static void send_data(int fd)
{
	char buf[MAX_BUF_LEN];

	memset((void *)buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "msg: %s:", "Hello world");
	printf("[Send Data] - [PARENT] pid: %u, %s\n", getpid(), buf);
	if(write(fd, buf, sizeof(buf)) == -1){
		perror("write()");
		return ;
	}
}

int main(int argc, char **argv)
{
	pid_t pid;
	int pipefd[2] = {0,};
	if(pipe(pipefd) == -1){
		perror("pipe()");
		return -1;
	}
	pid = fork();
	if(pid == 0){
		/*parent process*/
		close(pipefd[0]);
		send_data(pipefd[1]);
		close(pipefd[1]);
	}else if(pid > 0){
		/*child process*/
		close(pipefd[1]);
		recv_data(pipefd[0]);
		close(pipefd[0]);
	}else{
		perror("fork()");
		return -1;
	}
	return 0;
}
