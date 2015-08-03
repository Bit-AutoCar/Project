#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXLINE 1024

int OUTPUT_GO = 0;
int OUTPUT_LEFT = 0;
int OUTPUT_RIGHT = 0;
int OUTPUT_BACK = 0;

char *escapechar = "exit\n";

//////////////////여기부터 컨트롤 함수들../////////////////////////


typedef struct _button
{
	int buttontype;
	int buttonnum;
	int pushcheck;//push or release
}button;
char *message="init";
void get_buttondata(int fd,button *b)
{
	int i;
	char buffer[8];
	if((read(fd,buffer,8))>0)
	{
		b -> buttontype         = buffer[6];
		b -> buttonnum          = buffer[7];
		b -> pushcheck          = buffer[4];
		for(i=0;i<8;i++)
			printf("[%d] %d %d\n",buffer[6],buffer[7],buffer[4]);
	}
	else
		puts("read fail!");
}

void buttondecode(button *b)
{
	char *pushbuttonstring[9] = {"push-A","push-B","push-X","push-Y","push-LB","push-RB","push-BACK","push-START","push-GUIDE"};
	char *releasebuttonstring[9] = {"release-A","release-B","release-X","release-Y","release-LB","release-RB","release-BACK","release-START","release-GUIDE"};
	if(b -> pushcheck == 1)
		message = pushbuttonstring[b -> buttonnum];
	else if(b -> pushcheck == 0)
		message = releasebuttonstring[b -> buttonnum];
	//      puts(message);
}

void verticalpadpush(int status)
{
	switch(status)
	{
		case -1 :
			message= "down d-pad push";break;
		case  1 :
			message= "up d-pad push";break;
		case  0 :
			message= "release down or up button";
	}
}
char *horizonalpadpush(int status)
{
	switch(status)
	{
		case -1 :
			message = "right d-pad push";break;
		case  1 :
			message = "left d-pad push";break;
		case  0 :
			message = "release right or left button";
	}
}
void paddecode(button *b)
{
	if(b -> buttonnum == 6)
		horizonalpadpush(b -> pushcheck);
	else if(b -> buttonnum == 7)
		verticalpadpush(b -> pushcheck);
}

///////////////////////컨트롤 함수 끝/////////////////////////








































int main(int argc, char *argv[])
{
	char sendline[MAXLINE];
	char *haddr;
	int size;
	int port;
	static int s;
	static struct sockaddr_in server_addr;
	int len;
	static struct sockaddr_in fd_ser;

	int fd;//디바이스드라이버 디스크립터
        button buttoninfo;//버튼 정



	if(argc < 3)
	{
		printf("Usage: %s SERVER_ADDRESS TCP_PORT\n", argv[0]);
		return -1;
	}
	else
	{
		haddr = argv[1];
		port = atoi(argv[2]);
	}

	if((s = socket(PF_INET,SOCK_STREAM, 0)) < 0)
	{
		printf("Client: Can't open stream socket.\n");
		return -1;
	}

	bzero((char*)&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(haddr);
	server_addr.sin_port = htons(port);

	if(connect(s,(struct sockaddr*)&server_addr,
				sizeof(server_addr)) < 0)
	{
		printf("Client: can't connect to server.\n");
		return -1;
	}
	len = sizeof(fd_ser);
	if(getpeername(s, (struct sockaddr*)&fd_ser,
				(socklen_t*)&len) == 0)
	{
		printf("fd_ser IP : %s, Port: %u\n",
				inet_ntoa(fd_ser.sin_addr),
				(unsigned)ntohs(fd_ser.sin_port));
	}

	// 여기부터 신호보내기
	// 눌렀을때의 값을 sendline에 저장하고 strncmp 에 비교하는값을 디바이스에서 받는 신호랑 맞추면됨~



	if((fd=open("/dev/input/js0",O_RDONLY))<0)//디바이스드라이버 오픈
	{
		perror("can't open()");
	}



	while(1)
	{


		get_buttondata(fd,&buttoninfo);//버튼데이터 받아옴
		if(buttoninfo.buttontype == 1)//버튼인지,패드인지 확인
			buttondecode(&buttoninfo);//버튼일경우!
		else//not button
			paddecode(&buttoninfo);//패드일경우!

		strcpy(sendline,message);
		size = strlen(sendline);
		sendline[size] = '\0';

		if(strncmp(sendline, "up d-pad push", (size_t)size) == 0)
		{
			if(OUTPUT_GO == 0)
			{
				write(s, "up d-pad push", strlen("up d-pad push"));
				OUTPUT_GO = 1;
			}
		}
		else if(strncmp(sendline, "right d-pad push", (size_t)size) == 0)
		{
			if(OUTPUT_LEFT == 0)
			{
				write(s, "right d-pad push", strlen("right d-pad push"));
				OUTPUT_LEFT = 1;
			}
		}
		else if(strncmp(sendline, "left d-pad push", (size_t)size) == 0)
		{
			if(OUTPUT_RIGHT == 0)
			{
				write(s, "left d-pad push", strlen("left d-pad push"));
				OUTPUT_RIGHT = 1;
			}
		}
		else if(strncmp(sendline, "down d-pad push", (size_t)size) == 0)
		{
			if(OUTPUT_BACK == 0)
			{
				write(s, "down d-pad push", strlen("down d-pad push"));
				OUTPUT_BACK = 1;
			}
		}
		else if(strncmp(sendline, "release down or up button", (size_t)size) == 0)
		{
			if(OUTPUT_GO == 1)
			{
				write(s, "release down or up button", strlen("release down or up button"));
				OUTPUT_GO = 0;
			}
		}
		else if(strncmp(sendline, "release right or left button", (size_t)size) == 0)
		{
			if(OUTPUT_LEFT == 1)
			{
				write(s, "release right or left button", strlen("release right or left button"));
				OUTPUT_LEFT = 0;
			}	
		}
	//	else if(strncmp(sendline, "Stop_Right", (size_t)size) == 0)
	//	{
	//		if(OUTPUT_RIGHT == 1)
	//		{
	//			write(s, "Stop_Right", strlen("Stop_Right"));
	//			OUTPUT_RIGHT = 0;
	//		}
	//	}
	//	else if(strncmp(sendline, "Stop_Back", (size_t)size) == 0)
	//	{
	//		if(OUTPUT_BACK == 1)
	//		{
	//			write(s, "Stop_Back", strlen("Stop_Back"));
	//			OUTPUT_BACK = 0;
	//		}
	//	}
		printf("Send Signal = << %s >> \n", sendline);

	}
	close(s);
	return 0;
}
