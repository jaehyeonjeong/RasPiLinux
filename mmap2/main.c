#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>	//ftruncate() 사
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>	//mmap() 사용

#define MAX_BUF_LEN 4096	//LEN 버퍼 4096할당
#define SLEEP_UTIME 1000	//1000마이크로 세컨드(1초)
#define FORK_NUM 5

typedef struct anon_info{
	int pid;
	char Message[MAX_BUF_LEN];
}ANON_INFO;	//메세지를 전달하기위한 아이디 구조체?

typedef struct data_info{
	int size;
	char Message[MAX_BUF_LEN];
}DATA_INFO;	//메세지 내용 구조체?

static void print_help(const char *progname)
{
	printf("Usage: %s [-fa] (monitor|send Message)\n", progname);
	printf("	options:\n");
	printf("		-f (monitor|send Message) File_path \n");
	printf("		-a\n");
}	//메세지 옵션 정보 함수?

static int do_monitoring(const char *file_path)
{
	int fd;
	DATA_INFO *buf;
	DATA_INFO *tmp_buf;

	fd = open(file_path, O_CREAT | O_RDWR, 0644);	//-rw-r--r-- 파일로 열기
	if(fd == -1){
		perror("open()");
		return -1;
	}

	if(ftruncate(fd, sizeof(DATA_INFO)) == -1){	/*파일을 넣을 데이터 만큼 늘려놓는다. */
		perror("ftruncate()");
		close(fd);
		return -1;
	}

	buf = (DATA_INFO *)mmap(NULL, sizeof(DATA_INFO), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);	//버퍼 메모리 매핑
	if(buf == MAP_FAILED){
		perror("mmap()");
		close(fd);
		return -1;
	}

	buf->size = 0;	//버퍼 사이즈 0으로 초기화
	memset(buf->Message, 0, sizeof(char) * MAX_BUF_LEN);	//버퍼 메모리 사이즈 세팅

	close(fd);

	tmp_buf = (DATA_INFO *)malloc(sizeof(DATA_INFO));	//전환 버퍼 데이터 동적 할당?
	memset((void *)tmp_buf, 0, sizeof(DATA_INFO));
	memcpy((void *)tmp_buf, (const void *)buf, sizeof(DATA_INFO));	//buf 구조체에 있는 사이즈와 메세지등 데이터정보를 복사
	printf("[monitor] client said [%s], Message size: %d\n", tmp_buf->Message, tmp_buf->size);	//모니터 클라이언트 메세지 내용과 사이즈

	while(1){ //tmp_buf, buf의 사이즈 비교 및 메모리 할당 비교를 실시간으로 처리?
		if(sizeof(tmp_buf) < sizeof(buf)){
			free(tmp_buf);
			tmp_buf = (DATA_INFO *)malloc(sizeof(DATA_INFO));
			memset((void *)tmp_buf, 0, sizeof(DATA_INFO));
			memcpy((void *)tmp_buf, (const void *)buf, sizeof(DATA_INFO));
			printf("[monitor] client said [%s], Message size: %d\n", tmp_buf->Message, tmp_buf->size);
		}else{
			if(memcmp((const void *)tmp_buf, (const void *)buf, sizeof(DATA_INFO))){
				memcpy((void *)tmp_buf, (const void *)buf, sizeof(DATA_INFO));
				printf("[monitor] client said [%s], Message size: %d\n", tmp_buf->Message, tmp_buf->size);
			}
		}
		usleep(SLEEP_UTIME);
	}

	free(tmp_buf);
	munmap(buf, sizeof(DATA_INFO));
	return 0;
}

static int do_send_data(const char *file_path, const char *message)
{
	int fd;
	fd = open(file_path, O_RDWR);
	DATA_INFO *buf;

	if(fd == -1){
		perror("open()");
		return -1;
	}
	buf = (DATA_INFO *)mmap(NULL, sizeof(DATA_INFO), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(buf == MAP_FAILED){
		close(fd);
		perror("mmap()");
		return -1;
	}
	buf->size = sizeof(message);
	strncpy(buf->Message, message, sizeof(buf->Message));
	munmap(buf, sizeof(DATA_INFO));	//버퍼 구조체 메모리 해제
	close(fd);
	return 0;
}

ANON_INFO *mmap_init(void)
{
	ANON_INFO *info;
	info = mmap(NULL, sizeof(ANON_INFO), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if(info == MAP_FAILED){
		perror("mmap()");
		return NULL;
	}
	memset((void *)info, 0, sizeof(ANON_INFO));
	return info;
}

static int anon_send_data(int index, ANON_INFO *info)
{
	sleep(index + 2);
	info->pid = getpid();
	snprintf(info->Message, sizeof(info->Message), "anonymous test %d\n", index);
	//배열버퍼의 문자 형식을 저장하고 출력
	return 0;
}

static int anon_monitor(ANON_INFO *info)
{
	int n = 0;
	ANON_INFO local;

	memset((void *)&local, 0, sizeof(local));
	while(1){
		if(memcmp((const void *)&local, (const void *)info, sizeof(local))){
			printf("	* pid = %d, Message = %s\n", info->pid, info->Message);
			memcpy((void *)&local, (const void *)info, sizeof(local));
			n++;
			if(n == FORK_NUM) break;	//아이디의 일치 여부에 따른 메세지 출력후 FORK_NUM과 n의 수가 일치하는 경우 반복문 종료
		}
		usleep(SLEEP_UTIME);
	}
	munmap((void *)info, sizeof(ANON_INFO));
	return 0;
}

static int do_anon(void)
{
	int i;
	pid_t pid;	//프로세스 아이디 타입 데이터 선언
	ANON_INFO *info;
	printf("* START ANONYMOUS MODE \n");
	info = mmap_init();
	for(i = 0; i<FORK_NUM; i++){
		pid = fork();	//현재 프로세스에서 자식 프로세스를 생성하는 함수
		if(pid > 0){
			/*parent*/	//0보다 크면 부모 함수
		}else if(pid == 0){
			/*child*/
			anon_send_data(i, info);
			munmap((void *)info, sizeof(ANON_INFO));
			return 0;
		}else{
			perror("fork()");
			return -1;
		}
	}
	anon_monitor(info);
	printf("*	END ANONYMOUS NODE\n");
	for(i = 0; i < FORK_NUM; i++){
		pid = wait(NULL);
		if(pid == -1){
			perror("wait()");
			return -1;
		}
		printf("	[PID: %d] Terminate\n", pid);
	}
	return 0;
}

int main(int argc, char **argv)
{
	if(argc < 2){
		goto main_err;
	}
	switch(argv[1][1])
	{
		case 'f':
			if(argc < 4){
				goto main_err;
			}
			if(!strcmp((const char *)argv[2], "monitor")){
				do_monitoring((const char *)argv[3]);
			}else if(!strcmp((const char *)argv[2], "send")){
				if(argc < 5){
					goto main_err;
				}
				do_send_data((const char *)argv[4], (const char *)argv[3]);
			}else{
				goto main_err;
			}
			break;
		case 'a':
			/*anonymous mode*/
			do_anon();
			break;
		default:
			goto main_err;
	}
	return 0;
main_err:
	print_help(argv[0]);
	return -1;
}

