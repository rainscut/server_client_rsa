#pragma once

#include "targetver.h"

#include <stdio.h>
#include <cstdlib>
#include <tchar.h>
#include <memory>
#include <string>  
#include <iostream>

//#include <ppl.h>
//#include <future>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>

#pragma comment (lib, "Ws2_32.lib")

#include "mySocket.h"
#include "server.h"
#include "client.h"

#define SIMPLE_DATA 0x01

int sleepTimer = 100;

void processingMessageGetTotalSize ( const char* mes, UINT& total );
char processingMessageGetNeedAnswer ( std::string* mes );
std::string* generateAnswer ( std::string* mes, UINT& size );
unsigned int minCountByteForLength ( const char& mes );
void threadKeyHandle ( server* ser );

//RSA
int n = 299;
int e = 5;
int d = 53;

void encrypt_s ( std::unique_ptr<char [ ]>& data, int size, std::unique_ptr<char [ ]>& result );
void decrypt_s ( std::unique_ptr<char [ ]>& data, int size, std::unique_ptr<char [ ]>& result );
char encrypt_char ( unsigned char elem );
char decrypt_char ( unsigned char elem );