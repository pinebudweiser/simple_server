#include<WinSock2.h>
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

#define GET_CHAT		0x1000
#define SAVE_CHAT		0x1001
#define RECEIVE_CHAT	0x1002

void print_chat(char* ChatBuffer)
{
	printf("-------------------------------------------------------\n");
	printf("-------------------------------------------------------\n");
}

int main(int argc, char** argv)
{
	uint32_t IPAddress;
	uint16_t Port;
	SOCKET SocketDescriptor;
	WSADATA WsaData;
	struct sockaddr_in ChatServer;
	char buffer[256] = { 0, };
	int WsaRet;

	if (argc != 3)
	{
		printf("ex: chat_client <ip> <port>\n");
		return 1;
	}
	IPAddress = inet_addr(argv[1]);
	Port = htons(atoi(argv[2]));

	WsaRet = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (WsaRet != 0)
	{
		printf(" [Err] Failed initialize WsaData. reason : %d\n", WSAGetLastError());
		return 1;
	}
	SocketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (SocketDescriptor == INVALID_SOCKET)
	{
		printf(" [Err] Socket create failed. reason : %d\n", WSAGetLastError());
		return 1;
	}
	ChatServer.sin_family = AF_INET;
	ChatServer.sin_addr.S_un.S_addr = IPAddress;
	ChatServer.sin_port = Port;

	if (connect(SocketDescriptor, &ChatServer, sizeof(ChatServer)) == SOCKET_ERROR)
	{
		printf("[Err] Server Connection failed..\n");
		return 1;
	}

	while (1)
	{
		print_chat(buffer);
		printf("Input Your Message: ");
		scanf("%s", buffer);
		send(SocketDescriptor, buffer, strlen(buffer) , 0);
		system("cls");
	}

	return 0;
}