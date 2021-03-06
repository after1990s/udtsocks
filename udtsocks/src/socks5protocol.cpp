/*
 * socks5protocol.cpp
 *
 *  Created on: Nov 13, 2015
 *      Author: administrator
 */

#include "socks5protocol.h"

socks5protocol::socks5protocol() {


}

socks5protocol::~socks5protocol() {

}

int  socks5protocol::recv_socks5_request(UDTSOCKET sock, std::vector<unsigned char> &vec)
{
	int recved = 0;
	vec.clear();
	vec.resize(1024);
	recved += UDT::recv(sock, (char*)&vec[0], sizeof(socks5_request_t),0);
	socks5_request_t *req = (socks5_request_t *)&vec[0];
	if (req->atype == SOCKS5_ATTYPE_IPV4)
	{
		if (g_debug)
		{
			std::cout << "recv ipv4 req" << std::endl;
		}
		//ipv4 addr.

		recved += UDT::recv(sock, (char*)&vec[recved], sizeof(in_addr)+sizeof(uint16_t), 0);
	}
	else if(req->atype == SOCKS5_ATTYPE_DOMAIN)
	{
		if (g_debug)
		{
			std::cout << "recv domain req, stage 1, recv domain length." << std::endl;
		}
		//domain name
		recved += UDT::recv(sock, (char*)&vec[recved], 1, 0);
		int domainlen = vec[recved-1];
		if (g_debug)
		{
			std::cout << "recv domain req, stage 2, recv domain name." << std::endl;
		}
		recved += UDT::recv(sock, (char*)&vec[recved], domainlen, 0);
		if (g_debug)
		{
			std::cout << "recv domain req, stage 3, recv port number." << std::endl;
		}
		//+short for the port number.
		recved += UDT::recv(sock, (char*)&vec[recved], sizeof(short), 0);
	}
	else if (req->atype == SOCKS5_ATTYPE_IPV6)
	{
		//ipv6 not support
		return UDTSOCKET_FAIL;
	}
	else
	{
		//no such type
		return UDTSOCKET_FAIL;
	}
	return recved;
}
void socks5protocol::response_hello_with_no_auth(std::vector<unsigned char> &vec)
{
	vec.clear();
	vec.resize(2);
	vec[0] = 0x05;
	vec[1] = 0x00;
	return;
}
void socks5protocol::response_hello_with_fail(std::vector<unsigned char> &vec)
{
	vec.clear();
	vec.resize(2);
	vec[0] = 0x05;
	vec[1] = 0xff;
	return;
}
void output_content(std::vector<unsigned char> &t, int len)
{
	char buf[8] = {0};
	len = len > 16 ? 16 : len;
	for (auto i = 0; i<len; i++)
	{
		memset(buf, 0, 8);
		sprintf(buf, "0x%.2x", t[i]);
		std::cout << buf << " ";
	}
	std::cout<<"\n";

//	std::cout << "srv addr:";
//	struct sockaddr addrinf =  udtconfig::getserveraddr();
//	unsigned char *p = (unsigned char*)&addrinf;
//	if (p[0] == 0x00)
//	{
//		asm("int $0x03");
//	}
//	for (auto i=0; i<9; i++)
//	{
//		memset(buf, 0, 8);
//		sprintf(buf, "0x%.2x", (unsigned char)p[i]);
//		std::cout << buf << " ";
//	}
	std::cout<<std::endl;
}
void output_content(const char *t,int len)
{
	if (!g_debug)
		return;
	char buf[16] = {0};
	len = len > 16 ? 16 : len;
	for (auto i = 0; i<len; i++)
	{
		memset(buf, 0, 16);
		sprintf(buf, "0x%.2x", (unsigned char)t[i]);
		std::cout << buf << " ";
	}
	std::cout<<"\n";

//	std::cout << "srv addr:";
//	struct sockaddr addrinf =  udtconfig::getserveraddr();
//	unsigned char *p = (unsigned char*)&addrinf;
//
//	if (p[0] == 0x00)
//	{
//		asm("int $0x03");
//	}
//	for (auto i=0; i<sizeof(sockaddr); i++)
//	{
//		memset(buf, 0, 16);
//		sprintf(buf, "0x%.2x", (unsigned char)p[i]);
//		std::cout << buf << " ";
//	}
	std::cout<<std::endl;
}
