#pragma once
class client : public mySocket {
public:
	client ( int port = 3000, int bufLen = 256 );
	client ( char* server, int port = 3000, int bufLen = 256 );
	client ( char* server, char* port, char* bufLen );
	~client ( );

	bool ready ( );
	bool sendMessage ( std::string*& sendbuf, UINT& size, std::unique_ptr<char [ ]>& answer );
	void stop ( );
	void setServerName ( char* server );
private:
	void connectServer ( addrinfo* );

	char* server = nullptr;
	SOCKET connectSocket;
	std::mutex mtxConnect;
};

