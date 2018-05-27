#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#pragma comment(lib, "Ws2_32.lib")

#define MAX_LEN			256
#define GET_CHAT		0x1000
#define SAVE_CHAT		0x1001
#define RECEIVE_CHAT	0x1002

void make_chat_data(FILE* fp, char* send_buffer)
{
	char temp_buffer[100] = { 0 };
	char* string;
	strcpy(send_buffer, " ");
	while (!feof(fp))
	{
		fgets(temp_buffer, 100, fp);
		printf("string : %s", temp_buffer);
		strcat(send_buffer, temp_buffer);
	}
	printf("send_buffer : %s\n", send_buffer);
}

DWORD CALLBACK ServerThread(SOCKET lParam)
{
	char ClientMessage[256] = { 0, };
	char send_buffer_list[4096];
	char MessageCode[5];
	int ClientResult = 0;
	uint32_t MessageCodeInt;

	while (TRUE)
	{
		FILE* FileDescriptor = fopen("ChatLog.txt", "a+");

		ClientResult = recv(lParam, ClientMessage, sizeof(ClientMessage), 0);

		if (ClientResult > 0)
		{
		//	printf("[DBG] Bytes received: %d\n", ClientResult);
		}
		else if (ClientResult == 0)
		{
			printf("[DBG] Connection closed\n");
			break;
		}
		else
		{
			printf("[DBG] recv failed: %d\n", WSAGetLastError());
			break;
		}
		sscanf(ClientMessage, "%d:", &MessageCodeInt);
		//printf("MessageCodeInt : %d\n", MessageCodeInt);
		switch (MessageCodeInt)
		{
		case GET_CHAT:
			printf("You called GET_CHAT! Good Job :)\n");
			make_chat_data(FileDescriptor, send_buffer_list);
			send(lParam, send_buffer_list, sizeof(send_buffer_list), 0);
			fclose(FileDescriptor);
			break;
		case SAVE_CHAT:
			printf("[DBG] Client Message: %s\n", ClientMessage);
			fprintf(FileDescriptor, "%s\n", ClientMessage);
			fclose(FileDescriptor);
			break;
		case RECEIVE_CHAT:
			break;
		default:
			printf("[ERR] I DON'T KNOW THIS CODE\n");
		}
		fclose(FileDescriptor);
	}
	return 0;
}

int main(int argc, char** argv)
{
	SOCKET SocketDescriptor;
	WSADATA WsaData;
	struct sockaddr_in ChatServer;
	int WsaRet;

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

	while (1)
	{
		Sleep(1);
		if ((ClientDescriptor = accept(SocketDescriptor, &ClientSockAddr, &ClientAddrLen)) == -1)
		{
			printf("[Err] Accept failed : %d\n", WSAGetLastError());
			return 1;
		}
		CreateThread(NULL, 0, ServerThread, ClientDescriptor, NULL, NULL);
		Sleep(100);
	}
	closesocket(SocketDescriptor);
	WSACleanup(WsaRet);
	return 0;
}