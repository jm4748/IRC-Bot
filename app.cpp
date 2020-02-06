#include "app.h"

#include <iostream>


void App::Run()
{
	nick = "testbot";
	channel = "testchannel";

	WSADATA data;

	WSAStartup( MAKEWORD( 2, 2 ), &data );

	sock = socket( AF_INET, SOCK_STREAM, 0 );

	sockaddr_in addr;

	addr.sin_family      = AF_INET;
	addr.sin_port        = htons( 6667 );
	addr.sin_addr.s_addr = htonl( ( 78 << 24 ) | ( 129 << 16 ) | ( 202 << 8 ) | 9 );//irc.mibbit.net ip 78.129.202.9
	ZeroMemory( addr.sin_zero, 8 );

	if ( connect( sock, ( sockaddr* )&addr, sizeof( sockaddr ) ) == SOCKET_ERROR )
	{
		cout << "-could not connect" << endl;
	}
	else
	{
		cout << "-connected" << endl;
	}

	u_long arg = 1;

	ioctlsocket( sock, FIONBIO, &arg );

	char buffer[ 512 ];
	//ZeroMemory( buffer, 512 );

	int i = 0;

	bool a = true;
	bool b = true;//for setting up after connection
	bool c = false;

	running = true;
	while ( running )
	{
		int n;

		n = recv( sock, buffer + i, 512 - i, 0 );
		if ( n != SOCKET_ERROR )
		{
			i += n;

			while ( true )
			{
				message = string( buffer, i );

				size_t p;

				p = message.find( "\r\n" );
				if ( p != string::npos )
				{
					message = message.substr( 0, p + 2 );

					cout << message;

					Process();

					if ( b )//one-time thing
					{
						if ( message.find( "PONG" ) != string::npos )
						{
							Send( "USER " + nick + " 0 * :" + nick );
							Send( "JOIN #" + channel );

							b = false;
						}
					}

					string temp;

					temp = string( buffer, i );

					temp = temp.substr( p + 2, i - ( p + 2 ) );

					memcpy( buffer, temp.data(), i - ( p + 2 ) );

					i = i - ( p + 2 );
				}
				else
				{
					break;
				}
			}
		}

		//

		if ( a )
		{
			Send( "PASS *" );
			Send( "NICK " + nick );

			a = false;
		}
	}

	closesocket( sock );

	WSACleanup();
}


void App::Send( string message )
{
	message += "\r\n";

	cout << "sending: " << message;

	int sent = 0;

	while ( sent < message.size() )
	{
		int n;

		n = send( sock, message.c_str() + sent, message.size() - sent, 0 );
		if ( n == SOCKET_ERROR )
		{
			break;
		}

		sent += n;
	}
}


void App::Process()
{
	size_t p;

	p = message.find( "PRIVMSG" );
	if ( p != string::npos )
	{
		message = message.substr( p );

		p = message.find( ":" );

		message = message.substr( p + 1 );

		message = message.substr( 0, message.size() );

		if ( message[ 0 ] == '!' )
		{
			string temp;

			p = message.find( " " );
			if ( p != string::npos )
			{
				temp = message.substr( 1, p - 1 );

				if ( temp == "say" )
				{
					message = message.substr( p + 1, message.size() - ( p + 1 ) - 2 );

					SendMsg( message );
				}
				else if ( temp == "quit" )
				{
					Send( "QUIT" );

					running = false;
				}
				else if ( temp == "define" )
				{
					message = message.substr( p + 1, message.size() - ( p + 1 ) - 2 );

					p = message.find( " " );

					temp = message.substr( 0, p );

					message = message.substr( p + 1, message.size() - ( p + 1 ) );

					map[ temp ] = message;
				}
				else if ( temp == "definition" )
				{
					message = message.substr( p + 1, message.size() - ( p + 1 ) - 2 );

					p = message.find( " " );

					message = message.substr( 0, p );

					SendMsg( "-" + map[ message ] );
				}
				else if ( temp == "join" )
				{
					//you could add a game here
				}
			}
		}

		return;
	}

	p = message.find( "KICK" );
	if ( p != string::npos )
	{
		Send( "JOIN #" + channel );
	}

	p = message.find( "PING" );
	if ( p != string::npos )
	{
		message[ 1 ] = 'O';
		Send( message );
	}
}


void App::SendMsg( string message )
{
	Send( "PRIVMSG #" + channel + " :" + message );
}