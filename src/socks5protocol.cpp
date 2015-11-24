/*
 * socks5protocol.cpp
 *
 *  Created on: Nov 13, 2015
 *      Author: administrator
 */

#include "socks5protocol.h"

socks5protocol::socks5protocol() {
	// TODO Auto-generated constructor stub

}

socks5protocol::~socks5protocol() {
	// TODO Auto-generated destructor stub
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
		//ipv4 addr.
		recved += UDT::recv(sock, (char*)&vec[recved], sizeof(sockaddr)+sizeof(char), 0);
	}
	else if(req->atype == SOCKS5_ATTYPE_DOMAIN)
	{
		//domain name
		recved += UDT::recv(sock, (char*)&vec[recved], 1, 0);
		int domainlen = vec[recved-1];

		recved += UDT::recv(sock, (char*)&vec[recved], domainlen, 0);
		//+short for the port number.
		recved += UDT::recv(sock, (char*)&vec[recved], sizeof(short), 0);
	}
	else if (req->atype == SOCKS5_ATTYPE_IPV6)
	{
		//ipv6 not support
		return -1;
	}
	else
	{
		//no such type
		return -1;
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
