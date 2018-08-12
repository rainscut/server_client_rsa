#include "stdafx.h"
#include "mySocket.h"

int mySocket::bufLen = 0;

mySocket::mySocket ( int port, int bufLen ) {
	this->bufLen = bufLen;
	this->port = new char [ 5 ];
	_itoa_s ( port, this->port, strlen ( this->port ), 10 );
}

mySocket::mySocket ( char* port, char* bufLen ) {
	this->bufLen = atoi ( bufLen );
	this->port = new char [ 5 ];
	strcpy_s ( this->port, strlen ( this->port ), port );
}

mySocket::~mySocket ( ) {
	delete [ ] port;
}

bool mySocket::sendData ( SOCKET conn, std::string*& data, const UINT& size ) {
	if ( conn == INVALID_SOCKET ) {
		return false;
	}
	std::unique_ptr<char [ ]> encrypt_data;
	std::unique_ptr<char [ ]> temp ( new char [ size ] );
	memcpy ( temp.get ( ), data->data ( ), size );
	encrypt_s ( temp, ( int ) size, encrypt_data );
	UINT totalB = size;
	UINT countB = 0;
	int errCount = 0;
	do {
		auto minI = min ( totalB - countB, ( UINT ) bufLen );
		std::unique_ptr<char [ ]> substr ( new char [ minI ] );
		memcpy ( substr.get ( ), &encrypt_data [ countB ], minI );
		int iRes = send ( conn, substr.get ( ), minI, 0 );
		if ( iRes == SOCKET_ERROR ) {
			printf ( "send failed: %d\n", WSAGetLastError ( ) );
			errCount++;
			if ( errCount > 2 ) {
				return false;
			}
			std::this_thread::sleep_for ( std::chrono::milliseconds ( 30 ) );
		}
		else {
			countB += iRes;
		}
	} while ( countB != totalB );
	return true;
}

bool mySocket::recvData ( SOCKET conn, std::unique_ptr<char [ ]>& answer, UINT& size ) {
	if ( conn == INVALID_SOCKET ) {
		return false;
	}
	std::string decriptAnswer;
	UINT totalB = 0;
	UINT countB = 0;
	UINT errCount = 0;
	int resRecv = 0;
	UINT minCount4Length = 0;
	do {
		std::unique_ptr<char [ ]> recvbuf ( new char [ bufLen ] );
		resRecv = recv ( conn, recvbuf.get ( ), bufLen, 0 );
		if ( resRecv > 0 ) {
			for ( int i = 0; i < resRecv; i++ ) {
				decriptAnswer.push_back ( recvbuf [ i ] );
			}
			if ( minCount4Length == 0 ) {
				minCount4Length = minCountByteForLength ( decrypt_char ( decriptAnswer [ 0 ] ) );
				if ( minCount4Length == 0 ) {
					break;
				}
			}
			countB += resRecv;
			if ( totalB == 0 && countB >= minCount4Length ) {
				std::unique_ptr<char [ ]> tmp;
				std::unique_ptr<char [ ]> substr ( new char [ minCount4Length ] );
				memcpy ( substr.get ( ), decriptAnswer.data ( ), minCount4Length );
				decrypt_s ( substr, minCount4Length, tmp );
				processingMessageGetTotalSize ( tmp.get ( ), totalB );
			}
		}
		else if ( resRecv < 0 ) {
			errCount++;
			if ( errCount > 2 ) {
				decriptAnswer.clear ( );
				return false;
			}
			std::this_thread::sleep_for ( std::chrono::milliseconds ( 30 ) );
		}
	} while ( resRecv > 0 && countB < max ( minCount4Length, totalB ) );
	size = totalB;
	std::unique_ptr<char [ ]> substr ( new char [ size ] );
	memcpy ( substr.get ( ), decriptAnswer.data ( ), size );
	decrypt_s ( substr, size, answer );
	decriptAnswer.clear ( );
	return true;
}

void mySocket::setPort ( int port ) {
	this->port = new char [ 5 ];
	_itoa_s ( port, this->port, strlen ( this->port ), 10 );
}

void mySocket::setPort ( char* port ) {
	this->port = new char [ 5 ];
	strcpy_s ( this->port, strlen ( this->port ), port );
}

void mySocket::setBufferLength ( int length ) {
	this->bufLen = length;
}

void mySocket::setBufferLength ( char* length ) {
	this->bufLen = atoi ( length );
}