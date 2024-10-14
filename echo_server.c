#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define BUF_SIZE 1024
#define MAX_LOG_SIZE 64
void error_handling(char *message);
void reverse_message(char *message, int str_len, char * reversed_message);

struct log{
    char *clnt_ip;
    char *time_str;
};

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    char message[BUF_SIZE]; // 받은 메세지 저장
    int str_len, i;
	int logCount = 0;
    
    struct sockaddr_in serv_adr;
    struct sockaddr_in clnt_adr;
    socklen_t clnt_adr_sz;
    
    if(argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);   
    if(serv_sock == -1)
        error_handling("socket() error");
    
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 IP로부터 접속 허용
    serv_adr.sin_port = htons(atoi(argv[1])); // 포트번호 설정

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");
    
    for(;;)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        if(clnt_sock == -1)
            error_handling("accept() error");
        else{
     	char *clnt_ip = inet_ntoa(clnt_adr.sin_addr); // 클라이언트 IP 주소
		time_t t = time(NULL);
		struct tm *tm_info = localtime(&t); // 지역 시간으로 변환
        char time_str[20];
		strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info); // 시간 포맷팅, 인터넷 참고
      	printf("Connected client IP: %s  Time: %s \n", clnt_ip, time_str);

        // 로그 저장
        struct log[MAX_LOG_SIZE] logs;
        logs[logCount].clnt_ip = clnt_ip;
        logs[logCount].time_str = time_str;
        logCount++;
      	
	}    
        while((str_len = read(clnt_sock, message, BUF_SIZE - 1)) > 0)
        {
	        // printf("%d---- \n",str_len); 
            // 메시지를 역순으로 변환
            char reversed_message[BUF_SIZE];
			reverse_message(message, str_len, reversed_message);
            // 클라이언트에게 역순 메시지 전송
            write(clnt_sock, reversed_message, str_len);
        }
        close(clnt_sock);
    }

    close(serv_sock);
    return 0;
}

void reverse_message(char *message, int str_len, char * reversed_message)
{
	for(int i = 0; i < str_len-1; i++)
    	reversed_message[i] = message[str_len - i -2];
    reversed_message[str_len-1] = '\n'; // 개행 문자 추가
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
