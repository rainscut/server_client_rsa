#include "stdafx.h"
#include <Windows.h>

void processingMessageGetTotalSize ( const char* buff, UINT& total ) {
	switch ( buff [ 0 ] ) {
		case SIMPLE_DATA:
			{
				total = ( unsigned char ) ( buff [ 1 ] ) * 256 * 256 * 256 +
					( unsigned char ) ( buff [ 2 ] ) * 256 * 256 +
					( unsigned char ) ( buff [ 3 ] ) * 256 +
					( unsigned char ) ( buff [ 4 ] );
			}
		default:
			break;
	}
}

char processingMessageGetNeedAnswer ( std::string* mes ) {
	switch ( ( *mes ) [ 0 ] ) {
		case SIMPLE_DATA:
			{
				return ( *mes ) [ 5 ];
			}
		default:
			break;
	}
	return 0x00;
}

std::string* generateAnswer ( std::string* mes, UINT& size ) {
	switch ( ( *mes ) [ 0 ] ) {
		case SIMPLE_DATA:
			{
				return mes;
			}
		default:
			break;
	}
	return nullptr;
}

unsigned int minCountByteForLength ( const char& mes ) {
	switch ( mes ) {
		case SIMPLE_DATA:
			{
				return 5;
			}
		default:
			break;
	}
	return 0;
}

void threadKeyHandle ( server* ser ) {
	while ( true ) {
		if ( GetKeyState ( VK_ESCAPE ) < 0 ) {
			ser->stop ( );
			break;
		}
		std::this_thread::sleep_for ( std::chrono::milliseconds ( sleepTimer ) );
	}
}

void encrypt_s ( std::unique_ptr<char [ ]>& data, int size, std::unique_ptr<char [ ]>& result ) {
	result = std::unique_ptr<char [ ]> ( new char [ size ] );
	for ( int i = 0; i < size; i++ ) {
		result [ i ] = encrypt_char ( data [ i ] );
	}
}

void decrypt_s ( std::unique_ptr<char [ ]>& data, int size, std::unique_ptr<char [ ]>& result ) {
	result = std::unique_ptr<char [ ]> ( new char [ size ] );
	for ( int i = 0; i < size; i++ ) {
		result [ i ] = decrypt_char ( data [ i ] );
	}
}

char encrypt_char ( unsigned char elem ) {
	long cur = 1;
	for ( auto j = 0; j < e; j++ ) {
		cur = ( cur * elem ) % n;
	}
	return ( char ) cur;
}

char decrypt_char ( unsigned char elem ) {
	long cur = 1;
	for ( auto j = 0; j < d; j++ ) {
		cur = ( cur * elem ) % n;
	}
	return ( char ) cur;
}