//
//  CalcNodes.cpp
//  gtf
//
//  Created by David Gallardo on 31/05/16.
//  Copyright (c) 2016 GTF Development Group. All rights reserved.
//

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 7171

#include "RigNodes.h"

//#include <QtCore/QObject>
//#include <QtNetwork/QTcpServer>
//#include <QtNetwork/QTcpSocket>
//#include <QtDebug>


void CalcNumberNode::update()
{
    if(dirty)
    {
        
        for (gtf::NodeConnectionBase* con : outputConnections)
        {
            gtf::NodeConnectionI32 * numberCon = gtf::NodeConnectionI32::CAST(con);
            numberCon->data = number;
        }
        
        for (gtf::NodeConnectionBase* con : outputConnections[0]->output)
        {
            gtf::NodeConnectionI32 * numberCon = gtf::NodeConnectionI32::CAST(con);
            numberCon->isDirty = true;
        }
        
        outputConnections[0]->isReady = true;
        dirty = false;
    }
}

void CalcMathOpNode::update()
{
    if(inputConnections[0]->isDirty || inputConnections[1]->isDirty)
    {
        dirty = true;
    }
    
    if(dirty)
    {
        gtf::NodeConnectionI32 * inputA = gtf::NodeConnectionI32::CAST(inputConnections[0]);
        gtf::NodeConnectionI32 * inputB = gtf::NodeConnectionI32::CAST(inputConnections[1]);
        
        int readyInputs = 0;
        
        if(inputConnections[0]->input != nullptr &&
           inputConnections[0]->input->isReady)
        {
            gtf::NodeConnectionI32 const * inputASource = gtf::NodeConnectionI32::CAST(inputConnections[0]->input);
            inputA->data = inputASource->data;
            readyInputs++;
        }
        else
        {
            inputA->data = 0;
        }
        
        if(inputConnections[1]->input != nullptr &&
           inputConnections[1]->input->isReady)
        {
            gtf::NodeConnectionI32 const * inputBSource = gtf::NodeConnectionI32::CAST(inputConnections[1]->input);
            inputB->data = inputBSource->data;
            readyInputs++;
        }
        else
        {
            inputB->data = 0;
        }
        
        {
            inputA->isDirty = false;
            inputB->isDirty = false;
            gtf::NodeConnectionI32 * output = gtf::NodeConnectionI32::CAST(outputConnections[0]);
            
            number = output->data = MathOp(inputA->data, inputB->data);
            output->isReady = true; //(readyInputs == 2);
            
            for(auto otherOut : output->output)
            {
                otherOut->isDirty = true;
            }
            
            dirty = false;
        }
    }
}

void ComponentNode::update()
{
	if (dirty)
	{

		for (gtf::NodeConnectionBase* con : outputConnections)
		{
			gtf::NodeConnectionStr * numberCon = gtf::NodeConnectionStr::CAST(con);
			if (!result.empty())
			{
				numberCon->data = result;
			}
		}

		for (gtf::NodeConnectionBase* con : outputConnections[0]->output)
		{
			gtf::NodeConnectionStr * numberCon = gtf::NodeConnectionStr::CAST(con);
			numberCon->isDirty = true;
		}

		outputConnections[0]->isReady = true;
		dirty = false;
	}
}

void RigOpNode::update()
{
	if (inputConnections[0]->isDirty || inputConnections[1]->isDirty)
	{
		dirty = true;
	}

	if (dirty)
	{
		//gtf::NodeConnectionStr * inputA = gtf::NodeConnectionStr::CAST(inputConnections[0]);
		//gtf::NodeConnectionStr * inputB = gtf::NodeConnectionStr::CAST(inputConnections[1]);

		int readyInputs = 0;

		//if (inputConnections[0]->input != nullptr &&
		//	inputConnections[0]->input->isReady)
		//{
		//	gtf::NodeConnectionStr const * inputASource = gtf::NodeConnectionStr::CAST(inputConnections[0]->input);
		//	inputA->data = inputASource->data;
		//	readyInputs++;
		//}
		//else
		//{
		//	inputA->data = "";
		//}

		//if (inputConnections[1]->input != nullptr &&
		//	inputConnections[1]->input->isReady)
		//{
		//	gtf::NodeConnectionStr const * inputBSource = gtf::NodeConnectionStr::CAST(inputConnections[1]->input);
		//	inputB->data = inputBSource->data;
		//	readyInputs++;
		//}
		//else
		//{
		//	inputB->data = "";
		//}

		//inputA->isDirty = false;
		//inputB->isDirty = false;

		for (gtf::NodeConnectionBase* con : inputConnections)
		{
			gtf::NodeConnectionStr * strCon = gtf::NodeConnectionStr::CAST(con);
			if (strCon->input != nullptr &&
				strCon->input->isReady)
			{
				gtf::NodeConnectionStr const * inputASource = gtf::NodeConnectionStr::CAST(strCon->input);
				strCon->data = inputASource->data;
				readyInputs++;
			}
			else
			{
				strCon->data = "";
			}
			strCon->isDirty = false;
		}

		gtf::NodeConnectionStr * output = gtf::NodeConnectionStr::CAST(outputConnections[0]);
		
		result = output->data = RigOp(inputA->data, inputB->data);

		output->isReady = true; //(readyInputs == 2);

		for (auto otherOut : output->output)
		{
			otherOut->isDirty = true;
		}

		dirty = false;
	}
}

TCPNode::TCPNode()
{
	createTCP("thuy");
	//thuy = new TcpServer();
}

int TCPNode::createTCP(char* message)
{
	//======
	//client
	//======
	//WSADATA wsaData;
	//SOCKET ConnectSocket = INVALID_SOCKET;
	//struct addrinfo *result = NULL,
	//	*ptr = NULL,
	//	hints;
	//char *sendbuf = "this is a test";
	//char recvbuf[DEFAULT_BUFLEN];
	//int iResult;
	//int recvbuflen = DEFAULT_BUFLEN;

	//// Validate the parameters
	//char *adr = "192.168.1.6";
	////if (argc != 2) {
	////	printf("usage: %s server-name\n", argv[0]);
	////	return 1;
	////}

	//// Initialize Winsock
	//iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//if (iResult != 0) {
	//	printf("WSAStartup failed with error: %d\n", iResult);
	//	return 1;
	//}

	//ZeroMemory(&hints, sizeof(hints));
	//hints.ai_family = AF_UNSPEC;
	//hints.ai_socktype = SOCK_STREAM;
	//hints.ai_protocol = IPPROTO_TCP;

	//// Resolve the server address and port
	//iResult = getaddrinfo(adr, DEFAULT_PORT, &hints, &result);
	//if (iResult != 0) {
	//	printf("getaddrinfo failed with error: %d\n", iResult);
	//	WSACleanup();
	//	return 1;
	//}

	//// Attempt to connect to an address until one succeeds
	//for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

	//	// Create a SOCKET for connecting to server
	//	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
	//		ptr->ai_protocol);
	//	if (ConnectSocket == INVALID_SOCKET) {
	//		printf("socket failed with error: %ld\n", WSAGetLastError());
	//		WSACleanup();
	//		return 1;
	//	}

	//	// Connect to server.
	//	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	//	if (iResult == SOCKET_ERROR) {
	//		closesocket(ConnectSocket);
	//		ConnectSocket = INVALID_SOCKET;
	//		continue;
	//	}
	//	break;
	//}

	//freeaddrinfo(result);

	//if (ConnectSocket == INVALID_SOCKET) {
	//	printf("Unable to connect to server!\n");
	//	WSACleanup();
	//	return 1;
	//}

	//// Send an initial buffer
	//iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	//if (iResult == SOCKET_ERROR) {
	//	printf("send failed with error: %d\n", WSAGetLastError());
	//	closesocket(ConnectSocket);
	//	WSACleanup();
	//	return 1;
	//}

	//printf("Bytes Sent: %ld\n", iResult);

	//// shutdown the connection since no more data will be sent
	//iResult = shutdown(ConnectSocket, SD_SEND);
	//if (iResult == SOCKET_ERROR) {
	//	printf("shutdown failed with error: %d\n", WSAGetLastError());
	//	closesocket(ConnectSocket);
	//	WSACleanup();
	//	return 1;
	//}

	// Receive until the peer closes the connection
	//do {

	//	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	//	if (iResult > 0)
	//		printf("Bytes received: %d\n", iResult);
	//	else if (iResult == 0)
	//		printf("Connection closed\n");
	//	else
	//		printf("recv failed with error: %d\n", WSAGetLastError());

	//} while (iResult > 0);

	// cleanup
	//closesocket(ConnectSocket);
	//WSACleanup();

	//return 0;


	//======
	//server
	//======
	WSADATA wsaData;
	int iResult;

	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	//----------------------
	// Create a SOCKET for listening for
	// incoming connection requests.
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET) {
		wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.

	// Get the local hostname
	char szHostName[255];
	gethostname(szHostName, 255);
	struct hostent *host_entry;
	host_entry = gethostbyname(szHostName);
	char * szLocalIP;
	szLocalIP = inet_ntoa(*(struct in_addr *)*host_entry->h_addr_list);

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(szLocalIP);
	service.sin_port = htons(DEFAULT_PORT);

	if (bind(ListenSocket,
		(SOCKADDR *)& service, sizeof(service)) == SOCKET_ERROR) {
		wprintf(L"bind failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//----------------------
	// Listen for incoming connection requests.
	//on the created socket
	if (listen(ListenSocket, 1) == SOCKET_ERROR) {
		wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	wprintf(L"Waiting for client to connect...\n");

	//TIMEVAL tv = { 0 };
	//INT iReturnStatus = -1;
	//fd_set readFDs = { 0 };

	//while (ListenSocket)
	//{
		//Sleep(1);
		//tv.tv_usec = 2400000000; // microseconds
		//tv.tv_sec = 2400000000; // microseconds
		//FD_ZERO(&readFDs);
		//FD_SET(ListenSocket, &readFDs);
		//printf("select()\n");
		//iReturnStatus = select(0, &readFDs, NULL, NULL, &tv);

		//timeval timeout = { 1, 0 };
		//fd_set fds;
		//FD_ZERO(&fds);
		//FD_SET(ListenSocket, &fds);

		//select(ListenSocket + 1, &fds, NULL, NULL, &timeout);

		//if (FD_ISSET(ListenSocket, &fds) == 1)
		//{
			//ClientSocket = accept(ListenSocket, NULL, NULL);
			//if (ClientSocket == INVALID_SOCKET) {
			//	wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
			//	closesocket(ListenSocket);
			//	WSACleanup();
			//	return 1;
			//}
			//else
			//	wprintf(L"Client connected.\n");
			//send(ClientSocket, msg.c_str(), (int)strlen(msg.c_str()), 0);
		//}
	//}


	//ClientSocket = accept(ListenSocket, NULL, NULL);
	//if (ClientSocket == INVALID_SOCKET) {
	//	wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
	//	closesocket(ListenSocket);
	//	WSACleanup();
	//	return 1;
	//}
	//else
	//	wprintf(L"Client connected.\n");

	//No longer need server socket
	//closesocket(ListenSocket);

	//Receive until the peer shuts down the connection
	//do {
	//	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	//	if (iResult > 0) {
	//		printf("Bytes received: %d\n", iResult);

	//		// Echo the buffer back to the sender
	//		//iSendResult = send(ClientSocket, (char*)recvbuf, iResult, 0);
	//		iSendResult = send(ClientSocket, msg.c_str(), (int)strlen(msg.c_str()), 0);
	//		if (iSendResult == SOCKET_ERROR) {
	//			printf("send failed with error: %d\n", WSAGetLastError());
	//			closesocket(ClientSocket);
	//			WSACleanup();
	//			return 1;
	//		}
	//		printf("Bytes sent: %d\n", iSendResult);
	//		printf("Str sent: %s\n", recvbuf);
	//	}
	//	else if (iResult == 0)
	//		printf("Connection closing...\n");
	//	else {
	//		printf("recv failed with error: %d\n", WSAGetLastError());
	//		closesocket(ClientSocket);
	//		WSACleanup();
	//		return 1;
	//	}

	//} while (iResult > 0); 

	//sendbytes
	//int iSendResult;
	//iSendResult = send(ClientSocket, msg.c_str(), (int)strlen(msg.c_str()), 0);
	//if (iSendResult == SOCKET_ERROR) {
	//	printf("send failed with error: %d\n", WSAGetLastError());
	//	closesocket(ClientSocket);
	//	WSACleanup();
	//	return 1;
	//}

	// shutdown the connection since we're done
	//iResult = shutdown(ClientSocket, SD_SEND);
	//if (iResult == SOCKET_ERROR) {
	//	printf("shutdown failed with error: %d\n", WSAGetLastError());
	//	closesocket(ClientSocket);
	//	WSACleanup();
	//	return 1;
	//}

	// cleanup
	//closesocket(ClientSocket);
	//WSACleanup();

	return 0;

}

void TCPNode::update()
{
	if (inputConnections[0]->isDirty)
	{
		dirty = true;
	}

	if (dirty)
	{
		gtf::NodeConnectionStr * inputA = gtf::NodeConnectionStr::CAST(inputConnections[0]);

		int readyInputs = 0;

		if (inputConnections[0]->input != nullptr &&
			inputConnections[0]->input->isReady)
		{
			gtf::NodeConnectionStr const * inputASource = gtf::NodeConnectionStr::CAST(inputConnections[0]->input);
			
			msg = inputASource->data;
			inputA->data = msg;

			TIMEVAL tv = { 0 };
			INT iReturnStatus = -1;
			fd_set readFDs = { 0 };

			tv.tv_usec		= 10; // microseconds
			//tv.tv_sec		= 1; // seconds
			FD_ZERO(&readFDs);
			FD_SET(ListenSocket, &readFDs);
			iReturnStatus = select(0, &readFDs, NULL, NULL, &tv);

			switch (iReturnStatus)
			{
				case 0:
				{
					//wprintf(L"Waiting for client to connect...\n");
					break;
				}
				case 1:
				{
					ClientSocket = accept(ListenSocket, NULL, NULL);
					if (ClientSocket == INVALID_SOCKET) {
						wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
						closesocket(ListenSocket);
						WSACleanup();
						return;
					}
					else
					{
						char hostname[255];
						gethostname(hostname, 255);

						sockaddr_in client_addr{};
						int size = sizeof(client_addr);
						auto ret = getpeername(ClientSocket, (sockaddr*)&client_addr, &size);

						printf("Client connected %s %s %d.\n", hostname, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
					}

					int iSendResult;
					iSendResult = send(ClientSocket, msg.c_str(), (int)strlen(msg.c_str()), 0);
					if (iSendResult == SOCKET_ERROR) {
						printf("send failed with error: %d\n", WSAGetLastError());
						closesocket(ClientSocket);
						WSACleanup();
						return;
					}
					closesocket(ClientSocket);
					break;
				}
				default:
					break;
			}

			readyInputs++;
		}
		else
		{
			inputA->data = "";
		}
	}
}

//======================
//anhungxadieu namespace
//======================
struct InputTextCallback_UserData
{
	std::string*            Str;
	ImGuiInputTextCallback  ChainCallback;
	void*                   ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
	InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
		std::string* str = user_data->Str;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	}
	else if (user_data->ChainCallback)
	{
		// Forward to user callback, if any
		data->UserData = user_data->ChainCallbackUserData;
		return user_data->ChainCallback(data);
	}
	return 0;
}

bool anhungxadieu::InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

bool anhungxadieu::InputTextMultiline(const char* label, std::string* str, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return ImGui::InputTextMultiline(label, (char*)str->c_str(), str->capacity() + 1, size, flags, InputTextCallback, &cb_user_data);
}

bool anhungxadieu::InputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return ImGui::InputTextWithHint(label, hint, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}
