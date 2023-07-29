import time #타임모듈 불러오기
import serial #pyserial 모듈 불러오기

ser = serial.Serial(    #serial 객체 생성
        port='/dev/ttyAMA1',     #시리얼통신에 사용할 포트
        baudrate=115200,    #통신속도 지정
        parity=serial.PARITY_NONE,  #패리티 비트 설정방식
        stopbits=serial.STOPBITS_ONE,   #스톱비트 지정
        bytesize=serial.EIGHTBITS,  #데이터 비트수 지정
        timeout=1   #타입아웃 설정
        )

while True: #반복해서 숫자값을 입력받음, 이때 숫자가 아닌 값을 입력하면 다시 입력을 요구함
    degree = input('what degree do you want for servo?\n')
    if degree.isdigit():
        ser.write(degree.encode())
    else:
        print("you have to pass a number");


