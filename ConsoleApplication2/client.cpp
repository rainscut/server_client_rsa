#include "stdafx.h"
#include "client.h"

client::client ( int port, int bufLen ) :
	mySocket ( port, bufLen ),
	connectSocket ( INVALID_SOCKET ) { }

client::client ( char* server, int port, int bufLen ) :
	mySocket ( port, bufLen ),
	connectSocket ( INVALID_SOCKET ) {
	auto len = strlen ( server );
	this->server = new char [ len + 1 ];
	strcpy_s ( this->server, len + 1, server );
}

client::client ( char* server, char* port, char* bufLen ) :
	mySocket ( port, bufLen ),
	connectSocket ( INVALID_SOCKET ) {
	auto len = strlen ( server );
	this->server = new char [ len + 1 ];
	strcpy_s ( this->server, len + 1, server );
}

client::~client ( ) {
	stop ( );
	delete [ ] this->server;
}

bool client::ready ( ) {
	std::lock_guard<std::mutex> lock ( mtxConnect );
	if ( server == nullptr || port == nullptr || bufLen == 0 ) {
		return false;
	}
	struct addrinfo hints;
	ZeroMemory ( &hints, sizeof hints );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	struct addrinfo* result = nullptr;
	if ( getaddrinfo ( server, port, &hints, &result ) != 0 ) {
		printf ( "getaddrinfo failed: %d\n", WSAGetLastError ( ) );
		return false;
	}
	connectServer ( result );
	freeaddrinfo ( result );
	if ( connectSocket == INVALID_SOCKET ) {
		printf ( "Unable to connect to server!\n" );
		return false;
	}
	return true;
}

void client::connectServer ( addrinfo* result ) {
	for ( auto ptr = result; ptr != NULL; ptr = ptr->ai_next ) {
		connectSocket = socket ( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol );
		if ( connectSocket == INVALID_SOCKET ) {
			printf ( "socket failed: %d\n", WSAGetLastError ( ) );
			break;
		}
		if ( connect ( connectSocket, ptr->ai_addr, ( int ) ptr->ai_addrlen ) == SOCKET_ERROR ) {
			closesocket ( connectSocket );
			connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
}

bool client::sendMessage ( std::string*& sendbuf, UINT& size, std::unique_ptr<char [ ]>& answer ) {
	if ( connectSocket == INVALID_SOCKET ) {
		if ( !ready ( ) ) {
			return false;
		}
	}
	bool result = true;
	if ( sendData ( connectSocket, sendbuf, size ) ) {
		if ( processingMessageGetNeedAnswer ( sendbuf ) > 0x00 ) {
			size = 0;
			if ( !recvData ( connectSocket, answer, size ) ) {
				result = false;
			}
		}
	}
	else {
		result = false;
	}
	if ( shutdown ( connectSocket, SD_SEND ) == SOCKET_ERROR ) {
		printf ( "shutdown failed: %d\n", WSAGetLastError ( ) );
	}
	closesocket ( connectSocket );
	connectSocket = INVALID_SOCKET;
	return result;
}

void client::stop ( ) {
	closesocket ( connectSocket );
	connectSocket = INVALID_SOCKET;
}

void client::setServerName ( char* server ) {
	auto len = strlen ( server );
	this->server = new char [ len + 1 ];
	strcpy_s ( this->server, len + 1, server );
}