/*
 * udtconfig.cpp
 *
 *  Created on: Nov 7, 2015
 *      Author: administrator
 */

#include "udtconfig.h"
CONFIG udtconfig::m_config;
udtconfig::udtconfig() {
	// TODO Auto-generated constructor stub

}

udtconfig::~udtconfig() {
	// TODO Auto-generated destructor stub
}

void udtconfig::parseconfig()
{
	//demon
	return ;
}
struct addrinfo udtconfig::getserveraddr()
{
	return m_config.remote_server_addr;
}
std::string udtconfig::getlistenport()
{
	std::string s = m_config.listen_port;
	return s;
}
void udtconfig::setlistenport(char *port)
{
	m_config.listen_port = port;
}
void udtconfig::setserveraddr(char *addr, char* port)
{
	m_config.domain = addr;
	struct addrinfo hints = {0};
	hints.ai_family  = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo *res = NULL;
	getaddrinfo(addr, port, &hints, &res);

	memset (&m_config.remote_server_addr, 0, sizeof(m_config.remote_server_addr));
	memcpy (&m_config.remote_server_addr, res, sizeof(m_config.remote_server_addr));
	freeaddrinfo(res);
}
