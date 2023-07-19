#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SHARED_PATH "/home/user/.ssh/RasPiLinux/IPC/sysvMessage/sysv_example/pw_file"
#define PROJ_ID 1234		//프로젝트 아이디 선언

#define MAX_BUF_LEN 1024

typedef struct msgbuf {
	long mtype;
	char mtext[MAX_BUF_LEN];
}SYSV_BUF;

static void print_help(char *program_name)
{
	printf("Usage: %s (s mtype |r mtype)\n", program_name);
	// s와 r타입 선정
}

SYSV_BUF sysv_buf_setting(const char* msg, long mtype)	//버퍼 세팅
{
	SYSV_BUF buf;
	memset((void *)&buf, 0, sizeof(SYSV_BUF));
	strcpy(buf.mtext, msg);
	buf.mtype = mtype;
	return buf;
}

static int sysv_mqid_setting(int *mq_id)
{
	/*Gen ipc key*/
	key_t key;
	printf(" = Create IPC Key ");
	key = ftok(SHARED_PATH, PROJ_ID);	//조합할 파일 경로에 임의의 프로젝트 아이디로 ipc key값을 구함(해당 파일은 readable 해야한다.)
	if(key == -1){
		printf(" => FAIL \n");
		perror("ftok()");
		return -1;
	}
	printf(" => SUCCESS, key: %d\n", key);

	printf(" = Create Message Queue ID ");
	*mq_id = msgget(key, IPC_CREAT);	//Mseeage queue ID를 구하는 함수(key에 매치되는 message queue ID 없으면 생성)
	if(*mq_id == -1){
		printf(" => FAIL \n");
		perror("msgget()");
		return -1;
	}
	printf(" => SUCCESS, ID: %d\n", *mq_id);
	return 0;
}

int send_data(long mtype){
	int mq_id;
	char buf[MAX_BUF_LEN];
	SYSV_BUF mq_buf;

	if(sysv_mqid_setting(&mq_id) == -1){
		printf("FAIL setting mq id\n");
		return -1;
	}

	memset((void *)buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "Hello World");
	mq_buf = sysv_buf_setting(buf, mtype);

	printf(" = Send Message [%s] ", mq_buf.mtext);
	if(msgsnd(mq_id, (const void *)&mq_buf, sizeof(mq_buf.mtext), 0) == -1){	//메세지 전송 함수 -> 메세지 전송 처리
		printf(" => FAIL \n");
		perror("msgsnd()");
		return -1;
	}
	printf(" => SUCCESS \n");
	
	return 0;
}

int recv_data(long mtype){
	int mq_id;
	SYSV_BUF mq_buf;

	/*Gen ipc key*/
	if(sysv_mqid_setting(&mq_id) == -1){
		printf("FAIL setting mq id\n");
		return -1;
	}

	if(msgrcv(mq_id, (void *)&mq_buf, sizeof(mq_buf.mtext), mtype, 0) == -1){	//메세지 수신 함수 -> 메세지 수신 처리
		printf(" => FAIL \n");
		perror("msgrcv()");
		return -1;
	}

	printf(" = Recv Message => SUCCESS, Recv Data: %s\n", mq_buf.mtext);
	return 0;
}

int main(int argc, char **argv)
{
	long mtype;
	if(argc < 3){
		print_help(argv[0]);
		return -1;
	}

	int fd = open(SHARED_PATH, O_CREAT, 0644);
	close(fd);

	mtype = strtol((const char *)argv[2], NULL, 10); //문자열을 long 값으로 전환(10진수로 형태로 전환)

	if(!strcmp(argv[1], "s")){
		if(send_data(mtype) == -1){
			printf("메세지 전송을 실패하였습니다.\n");
			return -1;
		}
	}else if(!strcmp(argv[1], "r")){
		if(recv_data(mtype) == -1){
			printf("메세지 수신을 실패하였습니다.\n");
			return -1;
		}
	}else{
		print_help(argv[0]);
	}
	return 0;
}


	
