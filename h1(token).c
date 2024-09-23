#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <unistd.h>
#include <time.h>

#define SPI_CH 0
#define ADC_CH0 0
#define ADC_CS 8
#define SPI_SPEED 500000

#define LED_RED 7 //4
#define LED_GREEN 21 // 5
#define LED_BLUE 22   //6

int main(void){
	int adcValue=0, i;
	unsigned char buf[3];
	char adChannel = ADC_CH0 ;
	
	if(wiringPiSetup () == -1)
		return 1;

	pinMode(ADC_CS,OUTPUT);
	pinMode(LED_RED,OUTPUT);
	pinMode(LED_GREEN,OUTPUT);
	pinMode(LED_BLUE,OUTPUT);
	digitalWrite(LED_RED,0);
	digitalWrite(LED_GREEN,0);
	digitalWrite(LED_BLUE,0);

	
	if(wiringPiSPISetup(SPI_CH,SPI_SPEED) == -1){
		printf("wiringPi SPI Setup failed!\n");
		exit(0);
	}

	// 데이터 생성
	char report[50] = "208-34";
	for(int i = 0; i<5; i++){
		buf[0] = 0x06 | ((adChannel & 0x07)>>2);
		buf[1] = ((adChannel & 0x07)<<6);
		buf[2] = 0x00;
		
		digitalWrite(ADC_CS,0);
		
		wiringPiSPIDataRW(SPI_CH,buf,3);
		
		buf[1] = 0x0F & buf[1];
		adcValue = (buf[1] << 8) | buf[2];

		digitalWrite(ADC_CS,1);
		printf("Light ADC Value -> %d\n",adcValue);

		sprintf(report + strlen(report), "/%d/%d;", i+1, adcValue);
		usleep(5000000);
	}	

	// report 문자열에 담긴 데이터를 분석하여 평균값 계산
	int average = 0;
	int sum = 0;
	char *token;
	char *subtoken;
	char *saveptr1, *saveptr2;

	char *data_str = strtok(report, "/"); // 208-34 제거
	if (data_str != NULL) {
		for(token = strtok_r(data_str, ';', &saveptr1);, token != NULL; token = strtok_r(NULL, ';', &saveptr1)){

			subtoken = strtok_r(token, '/', &saveptr2); // 데이터 번호
			subtoken = strtok_r(NULL, '/', &saveptr2); // light 값

			sum += atoi(subtoken);
		}
	}
	
	average = sum / 5;

	// average 값에 따라서 LED 색상 변경
	if (average > 0 && average < 30) {
		digitalWrite(LED_RED,1);
		digitalWrite(LED_GREEN,0);
		digitalWrite(LED_BLUE,0);
		printf("It is dark\n");
	}
	else if (average >= 30 && average < 50) {
		digitalWrite(LED_RED,0);
		digitalWrite(LED_GREEN,1);
		digitalWrite(LED_BLUE,0);
		printf("It is medium light\n");
	}
	else {
		digitalWrite(LED_RED,0);
		digitalWrite(LED_GREEN,0);
		digitalWrite(LED_BLUE,1);
		printf("It is too bright\n");
	}


	return 0;
}
