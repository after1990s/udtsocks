/*
 * udtwrapper.cpp
 *
 *  Created on: Nov 25, 2015
 *      Author: administrator
 */

#include "udtwrapper.h"
extern const bool g_debug;
void setudtnonblocking(UDTSOCKET sock)
{
	bool bfalse = false;
	UDT::setsockopt(sock, 0, UDT_SNDSYN, &bfalse, sizeof(bool));
	UDT::setsockopt(sock, 0, UDT_RCVSYN, &bfalse, sizeof(bool));
}
void setsysnonblockingsend(int sock)
{
    int opts;
    opts=fcntl(sock,F_GETFL);
    if(opts<0)
    {
        perror("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts)<0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}
int recv_udtsock(UDTSOCKET sock, char * buf, int len, int flag)
{
	int recved = 0;
	while (recved != len)
	{
		int recvcount = UDT::recv(sock, buf, len, flag);
		if (recvcount == UDT::ERROR)
		{
			if (UDT::getlasterror().getErrorCode() == CUDTException::EASYNCRCV)
			{
				recved += recvcount;
				return recved;
			}
			else
			{
				return UDT::ERROR;
			}
		}
		recved += recvcount;
	}
	return recved;
}

int recv_syssock(int sock, char * buf, int len, int flag)
{
	int recved = 0;
		while (recved != len)
		{
			int recvcount = recv(sock, buf, len, flag);
			if (recvcount < 0)
			{
				if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
				{
					recved += recvcount;
					return recved;
				}
				else
				{
					return UDT::ERROR;
				}
			}
			recved += recvcount;
		}
		return recved;
}

int send_udtsock(UDTSOCKET sock, const char * buf, int len)
{
	int reversed = len;
	if (g_debug)
	{
		std::cout << "udtforwardclient sendto user:";
		output_content(buf, len);
	}
	while (reversed  > 0)
	{
		int writed = UDT::send(sock, buf, reversed, 0);
		if (writed == -1)
			break;
		reversed -= writed;
	}
	return len;
}
int send_syssock(int sock, const char * buf, int len)
{
	int reversed = len;
	if (g_debug)
	{
		std::cout << "udtforwardclient recv from user:";
		output_content(buf, len);
	}
	while (reversed > 0)
	{
		int writed = send(sock, buf, reversed, 0);
		if (writed == -1)
			break;
		reversed -= writed;
	}
	return len;
}

int access_map(std::map<int,int> map, int key)
{
	try
	{
		auto itr = map.find(key);
		if (itr != map.end())
		{
			return itr->second;
		}
	}
	catch (std::exception &i)
	{
		if (g_debug)
		{
			asm("int $3");
		}
	}
	if (g_debug)
	{
		std::cout << "warming: can't find usock in socket pair, ssock: " << key << std::endl;
	}
	return UDT::ERROR;
}

