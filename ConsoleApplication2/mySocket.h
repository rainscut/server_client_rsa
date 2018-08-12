#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>

class mySocket {
public:
	mySocket ( int port, int bufLen );
	mySocket ( char* port, char* bufLen );
	~mySocket ( );

	void setPort ( int port );
	void setPort ( char* port );
	void setBufferLength ( int length );
	void setBufferLength ( char* length );

	static bool sendData ( SOCKET conn, std::string*& data, const UINT& size );
	static bool recvData ( SOCKET conn, std::unique_ptr<char [ ]>& answer, UINT& size );

	char* port = nullptr;
	static int bufLen;
private:
};

