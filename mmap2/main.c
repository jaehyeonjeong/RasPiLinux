/**
 * @file main.c
 * @author Yoon Yeoung Jin (alwns28@kookmin.ac.kr)
 * @brief 
 * @version 0.1
 * @date 2022-06-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<fcntl.h>

typedef struct mmap_test_struct
{
    char name[20];
    int age;
    char hobby[30];
}MMAP_TEST_STRUCT;

#define FILE_PATH "/home/user/.ssh/RasPiLinux/mmap2/mmap_test"

/**
 * @brief MMAP_TEST_STRUCT 구조체 출력 함수
 * 
 * @param in 출력할 MMAP_TEST_STRUCT 구조체
 */
void yj_print_struct(MMAP_TEST_STRUCT in)
{
    printf("SETTING DATA:\n");
    printf("    * name: %s\n", in.name);
    printf("    * age: %d\n", in.age);
    printf("    * hobby: %s\n", in.hobby);
    printf("================================\n");
}

/**
 * @brief MMAP_TEST_STRUCT 구조체 셋팅 함수
 * 
 * @param in_name 이름
 * @param in_age 나이
 * @param in_hobby 취미
 * @param out 셋팅된 MMAP_TEST_STRUCT 구조체
 */
void yj_setting_struct(const char *in_name, const int in_age, const char *in_hobby, MMAP_TEST_STRUCT *out)
{
    strncpy(out->name, in_name, sizeof(out->name));
    out->age = in_age;
    strncpy(out->hobby, in_hobby, sizeof(out->hobby));
}

/**
 * @brief 파일 생성 함수
 * 
 * @param message 입력할 MMAP_TEST_STRUCT 구조체 
 * @return int 0: 성공, -1: 실패
 */
int yj_create_file(const MMAP_TEST_STRUCT *message)
{
    int fd = 0;
    fd = open((const char *)FILE_PATH, O_CREAT|O_WRONLY, 0644);
    if(fd == -1){
        printf("open fail: %s\n", strerror(errno));
        return -1;
    }
    if(write(fd, (const void *)message, sizeof(*message)) == -1){
        printf("write fail: %s\n",strerror(errno));
        return -1;
    }
    close(fd);
    return 0;
}

/**
 * @brief 파일 읽는 함수
 * 
 * @return int 0: 성공, -1: 실패
 */
int yj_read_file()
{
    MMAP_TEST_STRUCT out;

    int fd = 0;
    int out_bytes;
    fd = open((const char *)FILE_PATH, O_RDONLY);
    if(fd == -1){
        printf("open fail: %s\n", strerror(errno));
        return -1;
    }
    if((out_bytes = read(fd, (void *)&out, sizeof(out))) == -1){
        printf("read fail: %s\n", strerror(errno));
        return -1;
    }
    yj_print_struct(out);
    close(fd);
    return 0;
}

/**
 * @brief 파일에 있는 데이터를 mmap을 사용하여 메모리에 쓰는 함수
 * 
 */
void yj_write_mmap()
{
    int fd = 0;
    off_t mmap_offset = 0;
    struct stat buf;

    fd = open((const char *)FILE_PATH, O_RDWR);
    if(fd == -1){
        printf("open fail: %s\n", strerror(errno));
        return ;
    }
    fstat(fd, &buf);
    if(mmap((void *)NULL, (size_t)buf.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED,fd, mmap_offset) == MAP_FAILED){
        printf("mmap fail : %s\n", strerror(errno));
        return ;
    }
    close(fd);
}

/**
 * @brief mmap에서 데이터를 읽어와서 출력하는 함수
 * 
 * @param out mmap에서 읽어올 MMAP_TEST_STRUCT 구조체
 */
void yj_read_mmap(MMAP_TEST_STRUCT *out)
{
    int fd = 0;
    off_t mmap_offset = 0;
    struct stat buf;
    fd = open((const char *)FILE_PATH, O_RDWR);
    if(fd == -1){
        printf("open fail: %s\n", strerror(errno));
        return ;
    }
    fstat(fd, &buf);
    out = (MMAP_TEST_STRUCT *)mmap((void *)NULL, (size_t)buf.st_size, PROT_READ, MAP_SHARED, fd, mmap_offset);
    if(out == MAP_FAILED){
        printf("mmap fail : %s\n", strerror(errno));
        return ;
    }
    yj_print_struct(*out);
    if(munmap((void *)out, sizeof(out)) == -1){
        printf("munmap() fail: %s\n", strerror(errno));
    }

    close(fd);
}

int main(int argc, char **argv)
{
    MMAP_TEST_STRUCT in;
    MMAP_TEST_STRUCT out;

    printf("= STEP 1: Setting struct data \n");
    yj_setting_struct("YOON YEOUNG JIN", 26, "Play Room Escape game", &in);
    yj_print_struct(in);
    printf("= STEP 2: Write file \n");
    if(yj_create_file((const MMAP_TEST_STRUCT *)&in) == 0){
        printf("        ==> SUCCESS\n");
    } else {
        printf("        ==> FAIL\n");
    }
    yj_read_file();
    printf("= STEP 3: Write memory to use mmap() \n");
    yj_write_mmap();
    printf("================================\n");
    printf("= STEP 4: Read memory to use mmap() and other MMAP_TEST_STRUCT \n");
    yj_read_mmap(&out);
    return 0;
}
