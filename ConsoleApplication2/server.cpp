#include "stdafx.h"
#include "server.h"

server::server ( int port, int bufLen ) :
	mySocket ( port, bufLen ),
	listenSocket ( INVALID_SOCKET ) { }

server::server ( char* port, char* bufLen ) :
	mySocket ( port, bufLen ),
	listenSocket ( INVALID_SOCKET ) { }

server::~server ( ) {
	stop ( );
}

bool server::ready ( ) {
	std::lock_guard<std::mutex> lock ( mtxServer );
	if ( listenSocket != INVALID_SOCKET || port == nullptr || bufLen == 0 ) {
		return false;
	}
	int errCode = 0;
	struct addrinfo hints;
	ZeroMemory ( &hints, sizeof hints );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	struct addrinfo* result = nullptr;
	errCode = getaddrinfo ( 0, port, &hints, &result );
	if ( errCode != 0 ) {
		printf ( "getaddrinfo failed: %d\n", errCode );
		return false;
	}
	listenSocket = socket ( result->ai_family, result->ai_socktype, result->ai_protocol );
	if ( listenSocket == INVALID_SOCKET ) {
		printf ( "socket failed: %d\n", WSAGetLastError ( ) );
		freeaddrinfo ( result );
		return false;
	}
	if ( bind ( listenSocket, result->ai_addr, ( int ) result->ai_addrlen ) == SOCKET_ERROR ) {
		printf ( "bind failed: %d\n", WSAGetLastError ( ) );
		freeaddrinfo ( result );
		closesocket ( listenSocket );
		return false;
	}
	freeaddrinfo ( result );
	return true;
}

bool server::start ( ) {
	if ( listenSocket == INVALID_SOCKET ) {
		ready ( );
	}
	if ( listen ( listenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
		printf ( "listen failed: %d\n", WSAGetLastError ( ) );
		closesocket ( listenSocket );
		return false;
	}
	run = true;
	while ( run ) {
		thrConn.push_back ( std::thread ( threadClient, accept ( listenSocket, 0, 0 ) ) );
	}
	return true;
}

void server::stop ( ) {
	run = false;
	closesocket ( listenSocket );
	if ( thrMain.joinable ( ) ) {
		thrMain.join ( );
	}
	joinThreads ( );
	listenSocket = INVALID_SOCKET;
}

bool server::startAsync ( ) {
	if ( listenSocket == INVALID_SOCKET ) {
		ready ( );
	}
	if ( listen ( listenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
		printf ( "listen failed: %d\n", WSAGetLastError ( ) );
		closesocket ( listenSocket );
		return false;
	}
	run = true;
	thrMain = std::thread ( [ = ] ( ) {
		while ( run ) {
			thrConn.push_back ( std::thread ( &server::threadClient, accept ( listenSocket, 0, 0 ) ) );
		}
	} );
	return true;
}

void server::threadClient ( SOCKET clientSocket ) {
	if ( clientSocket == INVALID_SOCKET ) {
		printf ( "accept failed: %d\n", WSAGetLastError ( ) );
		return;
	}
	std::unique_ptr<char [ ]> recvbuf;
	UINT size;
	if ( recvData ( clientSocket, recvbuf, size ) ) {
		std::string strRecv;
		strRecv.resize ( size );
		for ( UINT i = 0; i < size; i++ ) {
			strRecv [ i ] = recvbuf [ i ];
		}
		if ( size && processingMessageGetNeedAnswer ( &strRecv ) > 0x00 ) {
			auto answer = generateAnswer ( &strRecv, size );
			sendData ( clientSocket, answer, size );
		}
		else {
			printf ( "server: " );
			for ( UINT i = 0; i < size; i++ ) {
				printf ( "%c", strRecv [ i ] );
			}
			printf ( "\n" );
		}
	}
	if ( shutdown ( clientSocket, SD_SEND ) == SOCKET_ERROR ) {
		printf ( "%u: shutdown failed: %d\n", ( unsigned int ) clientSocket, WSAGetLastError ( ) );
	}
	printf ( "%u close\n", ( unsigned int ) clientSocket );
	closesocket ( clientSocket );
}

void server::joinThreads ( ) {
	for ( auto it = thrConn.begin ( ); it != thrConn.end ( ); it++ ) {
		it->join ( );
	}
	thrConn.clear ( );
}