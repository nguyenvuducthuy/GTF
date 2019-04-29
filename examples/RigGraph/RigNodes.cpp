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
#define DEFAULT_PORT "7171"

#include "RigNodes.h"

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
			numberCon->data = result;
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
		gtf::NodeConnectionStr * inputA = gtf::NodeConnectionStr::CAST(inputConnections[0]);
		gtf::NodeConnectionStr * inputB = gtf::NodeConnectionStr::CAST(inputConnections[1]);

		int readyInputs = 0;

		if (inputConnections[0]->input != nullptr &&
			inputConnections[0]->input->isReady)
		{
			gtf::NodeConnectionStr const * inputASource = gtf::NodeConnectionStr::CAST(inputConnections[0]->input);
			inputA->data = inputASource->data;
			readyInputs++;
		}
		else
		{
			inputA->data = "";
		}

		if (inputConnections[1]->input != nullptr &&
			inputConnections[1]->input->isReady)
		{
			gtf::NodeConnectionStr const * inputBSource = gtf::NodeConnectionStr::CAST(inputConnections[1]->input);
			inputB->data = inputBSource->data;
			readyInputs++;
		}
		else
		{
			inputB->data = "";
		}

		{
			inputA->isDirty = false;
			inputB->isDirty = false;
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
}

TCPNode::TCPNode()
{
	number = createTCP("thuy");
}

int TCPNode::createTCP(char* message)
{
	//ADDRINFO hints = { 0 }, *result;
	//SOCKET client;


	//hints.ai_family = AF_UNSPEC;
	//hints.ai_socktype = SOCK_STREAM;
	//hints.ai_protocol = IPPROTO_TCP;

	//int addrinfoerror = getaddrinfo("10.0.131.77", "7171", &hints, &result);
	//if (addrinfoerror)
	//{
	//	printf("getaddrinfo returned error.");
	//	return -1;
	//}
	//client = socket(result->ai_family, result->ai_socktype, result->ai_protocol);


	//int connectresult = connect(client, result->ai_addr, result->ai_addrlen);
	//if (connectresult == SOCKET_ERROR)
	//{
	//	printf("socket connect error.");
	//	return -1;
	//}
	//::send(client, message, strlen(message), 0);
	//closesocket(client);
	//return 0;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	ADDRINFO hints, *result;
	ZeroMemory(&hints, sizeof(ADDRINFO));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(nullptr, "7171", &hints, &result);

	SOCKET listen_sock = socket(result->ai_family, SOCK_STREAM, result->ai_protocol);

	bind(listen_sock, result->ai_addr, (int)result->ai_addrlen);

	listen(listen_sock, SOMAXCONN);

	SOCKET client = accept(listen_sock, nullptr, nullptr);

	const int BUFF_LEN = 512;
	char buff[BUFF_LEN];
	int recv_size = recv(client, buff, BUFF_LEN, 0);
	send(client, buff, recv_size, 0);

	closesocket(client);
	//closesocket(listen_socket);
	WSACleanup();
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
			
			inputA->data = number == -1 ? "wrong" : "right";
			readyInputs++;
		}
		else
		{
			inputA->data = "";
		}
	}
}
