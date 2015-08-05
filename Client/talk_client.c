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
	int sticksensor;
}button;
char *message;
char flag = 0;
void get_buttondata(int fd,button *b) 
{
	int i;
	char buffer[8];
	if((read(fd,buffer,8))>0)
	{   
		b -> buttontype         = buffer[6];
		b -> buttonnum          = buffer[7];
		b -> pushcheck          = buffer[4];
		b -> sticksensor	= buffer[5];
		for(i=0;i<8;i++)
			printf("[%d]",buffer[i]);
			puts("");
	}   
	else
		puts("read fail!");
}

void buttondecode(button *b) 
{
	char *pushbuttonstring[9] = {"push-A","push-B","push-X","push-Y","push-LB","push-RB","push-END","push-START","push-GUIDE"};
	char *releasebuttonstring[9] = {"release-A","release-B","release-X","release-Y","release-LB","release-RB","release-END","release-START","release-GUIDE"};
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
			message= "push-dpad down";break;
		case  1 : 
			message= "push-dpad up";break;
		case  0 : 
			message= "release-dowb or up";
	}
}
char *horizonalpadpush(int status)
{
	switch(status)
	{
		case -1 :
			message = "push-dpad right";break;
		case  1 :
			message = "push-dpad left";break;
		case  0 :
			message = "release-dpad right or left";
	}
}
void verticalstickpush(int status)
{
	if(status < -110)
		message = "up stick push";
	else if(status > 110)
		message = "down stick push";
	else if(status < 0)
		message = "up stick release";
	else if(status > 0) 
		message = "down stick release";
}
void horizonalstickpush(int status)
{
	if(status < -110)
		message = "left stick push";
	else if(status > 110)
		message = "right stick push";
	else if(status < 0)
		message = "left stick release";
	else if(status > 0) 
		message = "right stick release";

}

void paddecode(button *b)
{
	if(b -> buttonnum == 6)
		horizonalpadpush(b -> pushcheck);
	else if(b -> buttonnum == 7)
		verticalpadpush(b -> pushcheck);
	else if(b -> buttonnum == 0)
		horizonalstickpush(b -> sticksensor);
	else if(b -> buttonnum == 1)
		verticalstickpush(b -> sticksensor);

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


	// 여기부터 신호보내기
	// 눌렀을때의 값을 sendline에 저장하고 strncmp 에 비교하는값을 디바이스에서 받는 신호랑 맞추면됨~



	if((fd=open("/dev/input/js0",O_RDONLY))<0)//디바이스드라이버 오픈
	{
		perror("can't open()");
	}

	while(1)
	{
		message = NULL;
		get_buttondata(fd,&buttoninfo);//버튼데이터 받아옴
		if(buttoninfo.buttontype == 1)//버튼인지,패드인지 확인
			buttondecode(&buttoninfo);//버튼일경우!
		else//not button
			paddecode(&buttoninfo);//패드일경우!

		if(message == NULL)
			continue;
		if(strncmp(message,"push-START",(size_t)size) == 0)
		{
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
				printf("Connect Successf! d_ser IP : %s, Port: %u\n", 
						inet_ntoa(fd_ser.sin_addr),
						(unsigned)ntohs(fd_ser.sin_port));
			}
			break;
		}
	}

	while(1)
	{
		message = NULL;
		get_buttondata(fd,&buttoninfo);//버튼데이터 받아옴
		if(buttoninfo.buttontype == 1)//버튼인지,패드인지 확인
			buttondecode(&buttoninfo);//버튼일경우!
		else//not button
			paddecode(&buttoninfo);//패드일경우!

		if(message == NULL)
			continue;
		strcpy(sendline,message);
		size = strlen(sendline);
		sendline[size] = '\0';


		if(strncmp(sendline,"push-END",(size_t)size) == 0)
		{
			write(s,"Disconnect", strlen("Disconnect"));
			break;
		}

		else if(strncmp(sendline, "push-Y", (size_t)size) == 0)
		{
			if(OUTPUT_GO == 0)
			{
				write(s, "GO", strlen("GO"));
				OUTPUT_GO = 1;
			}
		}
		else if(strncmp(sendline, "push-X", (size_t)size) == 0)
		{
			if(OUTPUT_LEFT == 0)
			{
				write(s, "LEFT", strlen("LEFT"));
				OUTPUT_LEFT = 1;
			}
		}
		else if(strncmp(sendline, "push-B", (size_t)size) == 0)
		{
			if(OUTPUT_RIGHT == 0)
			{
				write(s, "RIGHT", strlen("RIGHT"));
				OUTPUT_RIGHT = 1;
			}
		}
		else if(strncmp(sendline, "push-A", (size_t)size) == 0)
		{
			if(OUTPUT_BACK == 0)
			{
				write(s, "BACK", strlen("BACK"));
				OUTPUT_BACK = 1;
			}
		}

		else if(strncmp(sendline, "release-Y", (size_t)size) == 0)
		{
			if(OUTPUT_GO == 1)
			{
				write(s, "STOP_GO", strlen("STOP_GO"));
				OUTPUT_GO = 0;
			}
		}
		else if(strncmp(sendline, "release-X", (size_t)size) == 0)
		{
			if(OUTPUT_LEFT == 1)
			{
				write(s, "STOP_LEFT", strlen("STOP_LEFT"));
				OUTPUT_LEFT = 0;
			}	
		}
		else if(strncmp(sendline, "release-B", (size_t)size) == 0)
		{
			if(OUTPUT_RIGHT == 1)
			{
				write(s, "STOP_RIGHT", strlen("STOP_RIGHT"));
				OUTPUT_RIGHT = 0;
			}
		}
		else if(strncmp(sendline, "release-A", (size_t)size) == 0)
		{
			if(OUTPUT_BACK == 1)
			{
				write(s, "STOP_BACK", strlen("STOP_BACK"));
				OUTPUT_BACK = 0;
			}
		}

		else if(strncmp(sendline, "up stick push", (size_t)size) == 0)
		{
			if(OUTPUT_GO == 0)
			{
				write(s, "GO", strlen("GO"));
				OUTPUT_GO = 1;
				OUTPUT_LEFT = 0;
				OUTPUT_RIGHT = 0;
				OUTPUT_BACK = 0;
			}
		}
		else if(strncmp(sendline, "left stick push", (size_t)size) == 0)
		{
			if(OUTPUT_LEFT == 0)
			{
				write(s, "LEFT", strlen("LEFT"));
				OUTPUT_GO = 0;
				OUTPUT_LEFT = 1;
				OUTPUT_RIGHT = 0;
				OUTPUT_BACK = 0;
			}
		}
		else if(strncmp(sendline, "right stick push", (size_t)size) == 0)
		{
			if(OUTPUT_RIGHT == 0)
			{
				write(s, "RIGHT", strlen("RIGHT"));
				OUTPUT_GO = 0;
				OUTPUT_LEFT = 0;
				OUTPUT_RIGHT = 1;
				OUTPUT_BACK = 0;
			}
		}
		else if(strncmp(sendline, "down stick push", (size_t)size) == 0)
		{
			if(OUTPUT_BACK == 0)
			{
				write(s, "BACK", strlen("BACK"));
				OUTPUT_GO = 0;
				OUTPUT_LEFT = 0;
				OUTPUT_RIGHT = 0;
				OUTPUT_BACK = 1;
			}
		}

		else if(strncmp(sendline, "up stick release", (size_t)size) == 0)
		{
			if(OUTPUT_GO == 1)
			{
				write(s, "STOP_GO", strlen("STOP_GO"));
				OUTPUT_GO = 0;
			}
		}
		else if(strncmp(sendline, "left stick release", (size_t)size) == 0)
		{
			if(OUTPUT_LEFT == 1)
			{
				write(s, "STOP_LEFT", strlen("STOP_LEFT"));
				OUTPUT_LEFT = 0;
			}	
		}
		else if(strncmp(sendline, "right stick release", (size_t)size) == 0)
		{
			if(OUTPUT_RIGHT == 1)
			{
				write(s, "STOP_RIGHT", strlen("STOP_RIGHT"));
				OUTPUT_RIGHT = 0;
			}
		}
		else if(strncmp(sendline, "down stick release", (size_t)size) == 0)
		{
			if(OUTPUT_BACK == 1)
			{
				write(s, "STOP_BACK", strlen("STOP_BACK"));
				OUTPUT_BACK = 0;
			}
		}

		printf("Send Signal = << %s >> \n", sendline);

	}
	close(s);
	return 0;
}
