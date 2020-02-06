#pragma once


#include <WinSock2.h>

#include <string>
#include <map>


using namespace std;


#pragma comment( lib, "ws2_32.lib" )


class App
{
private:
	bool running;

	SOCKET sock;

	string message;

	map< string, string > map;

	string nick;
	string channel;

public:
	void Run();

	void Send( string message );

	void Process();

	void SendMsg( string message );
};