#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int sock; // 클라이언트 소켓 저장
    char message[BUF_SIZE]; // 서버로 보낼 메시지 저장하는 버퍼
    int str_len; // 송수신된 문자열의 길이 저장
    struct sockaddr_in serv_adr; // 서버의 주소 정보 저장 구조체

    if(argc != 3) { 
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    
    sock = socket(PF_INET, SOCK_STREAM, 0);   // 소켓 생성
    if(sock == -1)
        error_handling("socket() error");
    
    // 서버 주소 정보 초기화
    memset(&serv_adr, 0, sizeof(serv_adr)); // serv_adr의 모든 바이트를 0으로 초기화
    serv_adr.sin_family = AF_INET; // IPv4 사용
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]); // 서버 IP주소 설정
    serv_adr.sin_port = htons(atoi(argv[2])); // 서버 포트번호 설정

    if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error!");
    else
        puts("Connected...........");
    
    while(1) 
    {

        fputs("Input message(Q to quit): ", stdout); // 모니터 출력 요청 메세지
        fgets(message, BUF_SIZE, stdin); // 사용자의 입력을 message에 저장
        
        if(!strcmp(message,"q\n") || !strcmp(message,"Q\n")) // 입력이 q 또는 Q이면 탈출
            break;


        write(sock, message, strlen(message)); // 서버로 메시지 전송
        printf("Message to server: %s", message);
        str_len = read(sock, message, BUF_SIZE - 1); // 서버로부터 응답 수신
        message[str_len] = '\0'; // 문자열 종료 문자 추가
        printf("Message from server: %s", message);
    }
    
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
