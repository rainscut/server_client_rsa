#pragma once
#include <vector>

class server : public mySocket {
public:
	server ( int port = 3000, int bufLen = 256 );
	server ( char* port, char* bufLen );
	~server ( );

	bool ready ( );
	bool startAsync ( );
	bool start ( );
	void stop ( );
private:
	void joinThreads ( );
	static void threadClient ( SOCKET sock );

	bool run;
	SOCKET listenSocket;
	std::mutex mtxServer;
	std::vector<std::thread> thrConn;
	std::thread thrMain;
};

