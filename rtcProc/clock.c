#include <wiringPiI2C.h>	//wiringPi의 I2c통신용 라이브러리
#include <stdio.h>		// c언어의 표준 입출력 라이브러리
#include <unistd.h>		// sleep() 함수를 사용하기 위함

int decimalToBinary(int value){	//2진수를 십진수로 바꿔주는 함수
	int binary = (((value/10) << 4) + (value%10));
	return binary;
}

int binaryToDecimal(int value){	//10진수를 이진수로 바꿔주는 함수
	int decimal = ((value&240) >> 4) * 10 + (value&15);
	return decimal;
}

int main(){
	int hour;		// 시
	int minute;		// 분
	int second;		// 초
	
	printf("put a hour:");
	scanf("%d", &hour);	//시간을 입력받아 hour에 저장
	printf("put a minute:");
	scanf("%d", &minute);	//분을 입력받아 minute에 저장
	printf("put a second:");
	scanf("%d", &second);	//초를 입력받아서 second에 저장
	printf("You entered: %dH:%dM:%dS\n", hour, minute, second);	//입력받은 시간값 출력

	int fd = wiringPiI2CSetup(0x68);	//0x68의 주소를 가진 장치와 i2c통신을 개시
	wiringPiI2CWriteReg8(fd, 0x00, decimalToBinary(second));	//슬레이브의 0x00 레지스터에 초값을 기록
	wiringPiI2CWriteReg8(fd, 0x01, decimalToBinary(minute));	//슬레이브의 0x01 레지스터에 분값을 기록
	wiringPiI2CWriteReg8(fd, 0x02, decimalToBinary(hour));		//슬레이브의 0x02 레지스터에 시값을 기록

	int secs = binaryToDecimal(wiringPiI2CReadReg8(fd, 0x00));	//초 레지스터에서 값을 읽어들임
	int mm = binaryToDecimal(wiringPiI2CReadReg8(fd, 0x01));	//분 레지스터에서 값을 읽어들임
	int hh = binaryToDecimal(wiringPiI2CReadReg8(fd, 0x02));	//시 레지스터에서 값을 읽어들임

	while(1) {
		sleep(1);	//sec단위 30초 딜레이
		printf("The time is %2d:%02d:%02d\n", hh, mm, secs);	//읽어온 값 출력
	}

	return 0;
}	

	
