#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#pragma comment(lib, "Ws2_32.lib")

#define GET_CHAT		0x1000
#define SAVE_CHAT		0x1001
#define RECEIVE_CHAT	0x1002

DWORD CALLBACK ServerThread(LPVOID lParam)
{
	return 0;
}

int main(int argc, char** argv)
{
	SOCKET SocketDescriptor;
	WSADATA WsaData;
	struct sockaddr_in ChatServer;
	int WsaRet;
	FILE* FileDescriptor = fopen("ChatLog.txt", "a+");

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
	ChatServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	ChatServer.sin_port = htons((uint16_t)3456);

	if (bind(SocketDescriptor, &ChatServer, sizeof(ChatServer)) != 0)
	{
		printf("bind error\n");
		return 1;
	}
	if (listen(SocketDescriptor, 10) != 0)
	{
		printf("listen error\n");
		return 1;
	}

	SOCKET ClientDescriptor;
	struct sockaddr_in ClientSockAddr;
	int ClientAddrLen = sizeof(ClientSockAddr);
	int ClientResult = 0;
	char ClientMessage[1024];
	char MessageCode[5];
	uint32_t MessageCodeInt;

	if ((ClientDescriptor = accept(SocketDescriptor, &ClientSockAddr, &ClientAddrLen)) == -1)
	{
		printf("[Err] Accept failed : %d\n", WSAGetLastError());
		return 1;
	}

	while (1)
	{
		ClientResult = recv(ClientDescriptor, ClientMessage, strlen(ClientMessage), 0);

		if (ClientResult > 0)
		{
			printf("Bytes received: %d\n", ClientResult);
		}
		else if (ClientResult == 0)
		{
			printf("Connection closed\n");
		}
		else
		{
			printf("recv failed: %d\n", WSAGetLastError());
		}
		printf("Client Message : %s\n", ClientMessage);
		//sscanf(MessageCode, "%s:", ClientMessage);
		MessageCodeInt = atoi(MessageCode);

		switch (MessageCodeInt)
		{
		case GET_CHAT:
			break;
		case SAVE_CHAT:
			fputs(ClientMessage, FileDescriptor);
			send(ClientDescriptor, ClientMessage, strlen(ClientMessage), 0);
			fclose(FileDescriptor);
			break;
		case RECEIVE_CHAT:
			break;
		default:
			printf("[ERR] I DON'T KNOW THIS CODE\n");
		}
	}
	fclose(FileDescriptor);
	closesocket(SocketDescriptor);
	WSACleanup(WsaRet);
	return 0;
}