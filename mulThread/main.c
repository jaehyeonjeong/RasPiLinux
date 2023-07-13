#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define TEST_PATH "./"
#define AUTHER "Jeong Jae Hyeon "
#define USE_WAIT 0
#define USE_WAIT4 1

void jh_print_pid(void)
{
	pid_t pid;
	pid = getpid();
	printf("	* current process pid : %d\n", pid);
}

void jh_print_ppid(void)
{
	pid_t pid;
	pid = getppid();
	printf("	* parent process pid : %d\n", pid);
}

void jh_print_ruseage(const char *leader, const struct rusage *ru)
{
	const char *ldr = (leader == NULL) ? "" : leader;

	printf("%sCPU time (secs):	user-%.3f; system=%.3f\n", ldr,
			ru->ru_utime.tv_sec * ru->ru_utime.tv_usec / 1000000.0,
			ru->ru_stime.tv_sec * ru->ru_stime.tv_usec / 1000000.0);
	printf("%sMax resident set size:	%ld\n", ldr, ru->ru_maxrss);
	printf("%sIntegral shared memory:	%ld\n", ldr, ru->ru_ixrss);
	printf("%sIntegral unshared data:	%ld\n", ldr, ru->ru_idrss);
	printf("%sIntegral unshared stack:	%ld\n", ldr, ru->ru_isrss);
	printf("%sPage reclaims:		%ld\n", ldr, ru->ru_minflt);
	printf("%sPage faults:			%ld\n", ldr, ru->ru_majflt);
	printf("%sSwaps:			%ld\n", ldr, ru->ru_nswap);
	printf("%sBlock I/Os:	input=%ld; output=%ld\n", ldr,
			ru->ru_inblock, ru->ru_oublock);
	printf("%sSingals received:		%ld\n", ldr, ru->ru_nsignals);
	printf("%sIPC message:	sent=%ld; received=%ld\n", ldr, ru->ru_msgsnd, ru->ru_msgrcv);
	printf("%sContext switches:		volutary=%ld; "
			"involuntary=%ld\n", ldr, ru->ru_nvcsw, ru->ru_nivcsw);
}

int main(int argc, char **argv)
{
	int wstatus = 0;
	pid_t child_pid = 0;
	pid_t current_pid = 0;

	struct rusage usage;

	current_pid = fork();
	if(current_pid == -1){
		printf("fork() fail : %s\n", strerror(errno));
		return -1;
	} else if(current_pid > 0){
		/*parent process */
		printf(" [*] THIS IS PARENT PROCESS\n");
		jh_print_pid();
	} else {
		/*child process */
		printf(" [*] THIS IS CHILD PROCESS\n");
		jh_print_pid();
		jh_print_ppid();
		sleep(1);
		if(execl("/bin/ls", "ls", "-al", TEST_PATH, NULL) == -1){
			printf("execl() fail : %s\n", strerror(errno));
			exit(-1);
		}
	}

#if USE_WAIT
	child_pid = wait(&wstatus);
	if(child_pid == -1){
		printf("wait() fail: %s\n", strerror(errno));
		return -1;
	}
	if(WIFEXITED(wstatus)){
		printf("	* The child process(%d) terminated successfully. (CODE: %d)\n", child_pid, WEXITSTATUS(wstatus));
	} else {
		printf("	* The child process(%d) is not exited\n ", child_pid);
	}
#elif USE_WAIT4
	child_pid = wait4(current_pid, &wstatus, 0, &usage);
	if(child_pid == -1){
		printf("wait4() fail: %s\n", strerror(errno));
		return -1;
	}
	if(WIFEXITED(wstatus)){
		printf("	* The child process(%d) terminated sucessfully. (CODE: %d)\n ", child_pid, WEXITSTATUS(wstatus));
		jh_print_ruseage(AUTHER, (const struct rusage *)&usage);
	} else {
		printf("	* The child process(%d) is not exited\n ", child_pid);
	}
#endif

	return 0;
}

