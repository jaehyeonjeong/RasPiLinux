#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>
#include <sys/signal.h>

void ctrl_c_sighandler(int signum)
{
	printf("CTRL + C  (%d) \n", signum);	//Ctrl+C의 시그널은 2
}

int main(int argc, char **argv)
{
	signal(SIGINT, ctrl_c_sighandler);	//파라미터를 안받아도 되는건가?
	while(1){
		sleep(1);
	}
	return 0;
}
