#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#include "UDP_Interface.h"
#include <thread>
#include <string>
#include <stdexcept>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include "Packet.h"

using namespace std;

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512	//Max length of buffer
#define PORT 8888	//The port on which to listen for incoming data

int recvfromTimeOutUDP(SOCKET socket, long sec, long usec);
void thread_callback(bool* is_quitting, SOCKET* s, receive_event* pCallback, void** pContext);
ClientConnected* p;

//static std::recursive_mutex g_current_thread_mutex;

class UDP_Server : public API::Objects::UDPInterface
{
public:
	UDP_Server(int iPort, receive_event callback, SOCKET s_, void* pContext_);
	~UDP_Server() override;
	std::thread tCallback;
	bool is_quiting;
	//void thread_callback();
	SOCKET socket;
	receive_event callback;
	void* pCon;
	void** pointer_to_pContext;

	/*----------------------------------------
	Purpose: Sends raw data to the socket address given;
	Params:	pData - The raw data
			len - length of the raw data
			pDest - Pointer to the socket address we are sending it towards
	returns: none
	----------------------------------------*/
	virtual void Send(const void* pData, size_t len, const void* pDest) override;

	virtual void set_callback(receive_event r, void* pNewContext) override;

};
//  EAT FAT
class UDP_Client : public API::Objects::UDPInterface
{
public:
	UDP_Client(int iPort, receive_event r_, SOCKET s_, addrinfo* server_address, void* pContext_, string ID);
	~UDP_Client() override;
	SOCKET s;
	std::thread tCallback;
	receive_event callback;
	addrinfo* server_address;
	bool is_quiting;
	void* pCon;
	void** pointer_to_pContext;
	/*----------------------------------------
	Purpose: Sends raw data to the socket address given;
	Params:	pData - The raw data
			len - length of the raw data
			pDest - Pointer to the socket address we are sending it towards
	returns: none
	----------------------------------------*/
	virtual void Send(const void* pData, size_t len, const void* pDest) override;
	virtual void set_callback(receive_event r, void* pNewContext) override;


};

UDP_Client::UDP_Client(int iPort, receive_event r_, SOCKET s_, addrinfo* server_address_, void* pContext_, string ID)
	: s(s_)
	, callback(r_)
	, server_address(server_address_)
	, is_quiting(false)
	, pCon(pContext_)
{

	packet_pointer_t p = ClientConnected::create(ID);
	auto t = thread([](UDP_Client* pThis) {
		thread_callback(&pThis->is_quiting, &pThis->s, &pThis->callback, &pThis->pCon);
		}, this);
	tCallback.swap(t);
	Send(p.get(), sizeof(ClientConnected), server_address->ai_addr);

}


UDP_Client::~UDP_Client()
{
	is_quiting = true;
	tCallback.join();
	freeaddrinfo(server_address);
	if (closesocket(s) != 0)
	{
		printf("Server: Failed to close socket: %ld\n", WSAGetLastError());
	}

	if (WSACleanup() != 0)
	{
		printf("Server: WSACleanup() failed!Error code : %ld\n", WSAGetLastError());
	}
	else
	{
	}

}



void UDP_Client::Send(const void* pData, size_t len, const void* pDest_)
{
	const sockaddr* pDest = (const sockaddr*)pDest_;
	if (!pDest_) {
		pDest = this->server_address->ai_addr;
	}
	int num = sendto(s, (const char*)pData, len, 0, server_address->ai_addr, server_address->ai_addrlen);
	int y = 0;
}

void UDP_Client::set_callback(receive_event r, void* pNewContext)
{
	//Changes the callback and context
	callback = r;
	pCon = pNewContext;
}


UDP_Server::UDP_Server(int iPort, receive_event callback_, SOCKET s_, void* pContext_)
	: is_quiting(false)
	, socket(s_)
	, callback(callback_)
	, pCon(pContext_)
{

	auto t = thread([](UDP_Server* pThis) {
		thread_callback(&pThis->is_quiting, &pThis->socket, &pThis->callback, &pThis->pCon);
		}, this);
	tCallback.swap(t);
}

UDP_Server::~UDP_Server()
{
	is_quiting = true;
	tCallback.join();


	if (closesocket(socket) != 0)
	{
		printf("Server: closesocket() failed!Error code : %ld\n", WSAGetLastError());
	}
	else
	{
		printf("Server: closesocket() is OK...\n");
	}

	printf("Server: Cleaning up...\n");

	if (WSACleanup() != 0)
	{
		printf("Server: WSACleanup() failed!Error code : %ld\n", WSAGetLastError());
	}
	else
	{
		printf("Server: WSACleanup() is OK\n");
	}


}

void UDP_Server::Send(const void* pData, size_t len, const void* pDest_)
{
	const sockaddr* pDest = (const sockaddr*)pDest_;
	sendto(socket, (const char*)pData, len, 0, pDest, sizeof(*pDest));
}

void UDP_Server::set_callback(receive_event r, void* pNewContext)
{
	callback = r;
	pCon = pNewContext;
}


std::unique_ptr<API::Objects::UDPInterface> API::Factories::UDPFactory::create_server(int iPort, receive_event r, void* Context)
{
	WSADATA            wsaData;
	SOCKET s;
	struct sockaddr_in server;
	size_t server_size = sizeof(server);
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		throw runtime_error("Failed to start WSAStartup.");
	}

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		throw runtime_error("Failed to create socket.");
	}

	printf("Socket created.\n");

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(iPort);

	//Bind
	if (::bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf(" Bind failed with error code : % d ", WSAGetLastError());
		throw runtime_error("Failed to bind socket");
	}


	// getsockname(s, (SOCKADDR*)& server, (int*)&server_size);

	return std::make_unique<UDP_Server>(iPort, r, s, Context);
}

std::unique_ptr<API::Objects::UDPInterface> API::Factories::UDPFactory::create_client(string host, int iPort, receive_event r, void* Context, std::string ID)
{
	WSADATA wsaData;
	SOCKET s;
	struct sockaddr_in server_addr = { 0 };
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		throw runtime_error("Failed to start WSAStartup.");
	}

	struct addrinfo hints;
	struct addrinfo* pAddrinfo;  // Need to be freed. freeaddrinfo(pAddrinfo)

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	int get_result = getaddrinfo(host.c_str(), to_string(iPort).c_str(), &hints, &pAddrinfo);
	if (get_result != 0 || pAddrinfo == NULL)
	{
		throw runtime_error(("invalid address or port: \"" + host + ":" + to_string(iPort) + "\"").c_str());
	}


	//server_addr.sin_addr.s_addr = inet_addr("94.13.44.172");
	//server_addr.sin_port = htons(iPort);
	//server_addr.sin_family = AF_INET;

	//Create a socket   
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		throw runtime_error("Failed to create socket.");
	}

	//SOCKADDR_IN ReceiverAddr;
	//ReceiverAddr.sin_family = AF_INET;
	//ReceiverAddr.sin_port = htons(iPort);
	//ReceiverAddr.sin_addr.s_addr = inet_addr("eric"); // "127.0.0.1");

	//int n = sendto(s, "hi", 3, 0, (SOCKADDR*)& ReceiverAddr, sizeof(ReceiverAddr));

	//struct sockaddr* sa;
	//sa = (sockaddr*)malloc(pAddrinfo->ai_addrlen);
	//memcpy(sa, pAddrinfo->ai_addr, pAddrinfo->ai_addrlen);
	//size_t salen = pAddrinfo->ai_addrlen;
	//freeaddrinfo(pAddrinfo);

	//int rc = sendto(s, "hi", 3, 0, sa, salen);
	//sendto(s, (const char*)"Hi", 3, 0, pAddrinfo->ai_addr, pAddrinfo->ai_addrlen);
	////// connect to server 
	//if (connect(s, (struct sockaddr*) & server_addr, sizeof(server_addr)) < 0)
	//{
	//	printf("\n Error : Connect Failed \n");
	//	throw runtime_error("Failed to connect socket");
	//}

	return std::make_unique<UDP_Client>(iPort, r, s, pAddrinfo, Context, ID);
}

int recvfromTimeOutUDP(SOCKET socket, long sec, long usec)
{
	// Setup timeval variable
	timeval timeout;
	timeout.tv_sec = sec;
	timeout.tv_usec = usec;
	// Setup fd_set structure
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(socket, &fds);
	// Return value:
	// -1: error occurred
	// 0: timed out
	// > 0: data ready to be read
	return select(0, &fds, 0, 0, &timeout);
}


void thread_callback(bool* is_quitting, SOCKET* socket, receive_event* pCallback, void** pointer_to_pContext)
{
	// this would run in a thread.
	int bytes_received = 5;
	char buf[BUFLEN];
	sockaddr_in sender_addr;
	int sender_addr_size = sizeof(sender_addr);
	while (!(*is_quitting))
	{
		// Setup timeval variable
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 500000;

		// Setup fd_set structure
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(*socket, &fds);


		// -1: error occurred
		// 0: timed out
		// > 0: data ready to be read
		int iSelected = select(0, &fds, 0, 0, &timeout);
		if (iSelected < -1) {
			// tell me why trowing is bad?
			return;
			throw runtime_error("socket error");
		}
		if (iSelected == 0) {
			// timeout of select,  no data recevied
			continue;
		}
		// got data.
		//printf("***GOT DATA***\n");
		bytes_received = recvfrom(*socket, buf, BUFLEN, 0, (SOCKADDR*)&sender_addr, &sender_addr_size);
		if (bytes_received > 0)
		{
			(*pCallback)(buf, bytes_received, &sender_addr, sender_addr_size, *pointer_to_pContext);
		}
		else if (bytes_received <= 0)
		{
			// printf("Server: Connection closed with error code : %ld\n", WSAGetLastError());
			// throw runtime_error("Socket error?");
			return;  // TODO:  Log this as error
		}
		else
		{
			//			printf("Server: recvfrom() failed with error code : %d\n",WSAGetLastError());
			throw runtime_error("Failed to get data");
		}
	}
}

