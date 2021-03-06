#include "stdafx.h"

void readInputData ( int& argc, char**& argv, server*& ser, client*& cl );
std::string* lead_up ( char* buff, bool& a_wait, UINT& size );
void writeSizeBuff ( UINT size, char*& info );
void processingClient ( client*& cl, char type );

int main ( int argc, char** argv ) {
	if ( WSAStartup ( MAKEWORD ( 2, 2 ), &WSADATA ( ) ) != 0 ) {
		printf ( "WSAStartup failed: %d\n", WSAGetLastError ( ) );
		system ( "pause" );
		return 0;
	}
	std::thread* tHKey = nullptr;
	server* ser = nullptr;
	client* cl = nullptr;
	readInputData ( argc, argv, ser, cl );
	if ( ser ) {
		tHKey = new std::thread ( threadKeyHandle, ser );
		ser->startAsync ( );
	}
	if ( cl ) {
		processingClient ( cl, SIMPLE_DATA );
		cl->stop ( );
	}
	if ( tHKey ) {
		tHKey->join ( );
	}
	WSACleanup ( );
	system ( "pause" );
	return 0;
}

void processingClient ( client*& cl, char type ) {
	switch ( type ) {
		case SIMPLE_DATA:
			{
				bool a_wait = false;
				while ( true ) {
					std::unique_ptr<char [ ]> buff ( new char [ 1024 ] );
					printf ( "Enter \"exit\" to complete, \"-\" to wait answer, \"+\" to skip answer.\n" );
					printf ( "Enter message:\n" );
					std::cin.getline ( buff.get ( ), 1024 );
					if ( strlen ( buff.get ( ) ) == 0 || !strcmp ( buff.get ( ), "exit" ) ) {
						printf ( "Exit.\n" );
						break;
					}
					else if ( !strcmp ( buff.get ( ), "-" ) ) {
						a_wait = false;
						continue;
					}
					else if ( !strcmp ( buff.get ( ), "+" ) ) {
						a_wait = true;
						continue;
					}
					printf ( "Message processing...\n" );
					UINT size = 0;
					auto res = lead_up ( buff.get ( ), a_wait, size );
					for ( UINT i = 0; i < size; i++ ) {
						printf ( "%c", ( *res ) [ i ] );
					}
					printf ( "\n" );
					std::unique_ptr<char [ ]> answer = nullptr;
					if ( cl->sendMessage ( res, size, answer ) ) {
						if ( ( *res ) [ 5 ] > 0x00 ) {
							printf ( "answer: " );
							for ( UINT i = 0; i < size; i++ ) {
								printf ( "%c", answer [ i ] );
							}
							printf ( "\n" );
						}
					}
					res->clear ( );
				}
			}
		default:
			break;
	}
}

std::string* lead_up ( char* buff, bool& a_wait, UINT& size ) {
	char* info = new char [ 6 ];
	info [ 0 ] = SIMPLE_DATA;
	info [ 5 ] = a_wait ? 0x01 : 0x00;
	size = strlen ( buff ) + 6;
	writeSizeBuff ( size, info );
	std::string* res = new std::string ( );
	for ( size_t i = 0; i < size; i++ ) {
		if ( i < 6 ) {
			res->push_back ( info [ i ] );
		}
		else {
			res->push_back ( buff [ i - 6 ] );
		}
	}
	return res;

}

void writeSizeBuff ( UINT size, char*& info ) {
	info [ 1 ] = size >> 24 & 0xFF;
	info [ 2 ] = size >> 16 & 0xFF;
	info [ 3 ] = size >> 8 & 0xFF;
	info [ 4 ] = size & 0xFF;
}

void readInputData ( int& argc, char**& argv, server*& ser, client*& cl ) {
	for ( int i = 1; i < argc; i++ ) {
		auto tmp = argv [ i ];
		if ( !strcmp ( tmp, "--on_server" ) ) {
			printf ( "Starting server...\n" );
			ser = new server ( );
			int j = i + 1;
			for ( ; j < argc; j += 2 ) {
				auto tmp = argv [ j ];
				if ( !strcmp ( tmp, "-p" ) || !strcmp ( tmp, "--port" ) ) {
					ser->setPort ( argv [ j + 1 ] );
				}
				else if ( !strcmp ( tmp, "-lb" ) || !strcmp ( tmp, "--length_buffer" ) ) {
					ser->setBufferLength ( argv [ j + 1 ] );
				}
				else {
					break;
				}
			}
			i = j - 1;
			printf ( "Start server.\n" );
			break;
		}
		else if ( !strcmp ( tmp, "--connect" ) ) {
			printf ( "Starting client...\n" );
			cl = new client ( );
			int j = i + 1;
			for ( ; j < argc; j += 2 ) {
				auto tmp = argv [ j ];
				if ( !strcmp ( tmp, "-p" ) || !strcmp ( tmp, "--port" ) ) {
					cl->setPort ( argv [ j + 1 ] );
				}
				else if ( !strcmp ( tmp, "-lb" ) || !strcmp ( tmp, "--length_buffer" ) ) {
					cl->setBufferLength ( argv [ j + 1 ] );
				}
				else if ( !strcmp ( tmp, "-s" ) || !strcmp ( tmp, "--server" ) ) {
					cl->setServerName ( argv [ j + 1 ] );
				}
				else {
					break;
				}
			}
			i = j - 1;
			printf ( "Start client.\n" );
		}
		else if ( !strcmp ( tmp, "-h" ) || !strcmp ( tmp, "--help" ) ) {
			printf ( "Help server_client_rsa:\
			\n--on_server: start server.\
			\nnext you need to specify the parameters: port(-p) and the length_buffer(-lb).\
			\notherwise there will be default parameters: port = 3000, length_buffer = 256. \
			\n--connect: start client.\
			\nnext you need to specify the parameters: server(-s), port(-p) and the length_buffer(-lb).\
			\notherwise there will be default parameters: port = 3000, length_buffer = 256. \
			\n--port / -p: port\
			\n--length_buffer / -lb: the length of the buffer\
			\n--server / -s: server to connect\n" );
		}
	}
}