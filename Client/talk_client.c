#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 1024

char *escapechar = "exit\n";

int main(int argc, char *argv[])
{
	char sendline[MAXLINE], recvline[MAXLINE + 1];
	char *haddr;
	int size;
	int port;
	pid_t pid;
	static int s;
	static struct sockaddr_in server_addr;
	int len;
	static struct sockaddr_in fd_ser;

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

	while(1)
	{
		size = strlen(sendline);
		sendline[size] = '\0';

		if(strncmp(sendline, "Go", (size_t)size) == 0)
		{
			write(s, "Go", strlen("Go"));
		}
		else if(strncmp(sendline, "Left", (size_t)size) == 0)
		{
			write(s, "Left", strlen("Left"));
		}
		else if(strncmp(sendline, "Right", (size_t)size) == 0)
		{
			write(s, "Right", strlen("Right"));
		}
		else if(strncmp(sendline, "Back", (size_t)size) == 0)
		{
			write(s, "Back", strlen("Back"));
		}
		else if(strncmp(sendline, "Stop_Go", (size_t)size) == 0)
		{
			write(s, "Stop_Go", strlen("Stop_Go"));
		}
		else if(strncmp(sendline, "Stop_Left", (size_t)size) == 0)
		{
			write(s, "Stop_Left", strlen("Stop_Left"));
		}
		else if(strncmp(sendline, "Stop_Right", (size_t)size) == 0)
		{
			write(s, "Stop_Right", strlen("Stop_Right"));
		}
		else if(strncmp(sendline, "Stop_Back", (size_t)size) == 0)
		{
			write(s, "Stop_Back", strlen("Stop_Back"));
		}
		printf("Send Signal = << %s >> \n", sendline);
		
		if(strncmp(sendline, escapechar, 4) == 0)
		{
			kill(pid, SIGQUIT);
			break;
		}

	}
	close(s);
	return 0;
}
