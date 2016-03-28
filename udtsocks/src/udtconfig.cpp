/*
 * udtconfig.cpp
 *
 *  Created on: Nov 7, 2015
 *      Author: administrator
 */

#include "udtconfig.h"
CONFIG udtconfig::m_config;
udtconfig::udtconfig() {


}

udtconfig::~udtconfig() {
	
}

void udtconfig::parseconfig()
{
	//demon
	return ;
}
struct sockaddr udtconfig::getserveraddr()
{
	return *m_config.remote_server_addr.ai_addr;
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
	static bool runonce = false;
	if (runonce)
	{
		perror("udtconfig::setserveraddr can only call once, due to prevent memory leak, pause.");
		pause();
	}
	if (!runonce)
	{
		runonce = true;
	}
	m_config.domain = addr;
	struct addrinfo hints = {0};
	hints.ai_family  = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	struct addrinfo *res = NULL;
	if (getaddrinfo(addr, port, &hints, &res) != 0)
	{
		perror("get server address failed.");
		return ;
	}

	memset (&m_config.remote_server_addr, 0, sizeof(m_config.remote_server_addr));
	memcpy (&m_config.remote_server_addr, res, sizeof(m_config.remote_server_addr));

	struct sockaddr *paddr = new struct sockaddr;//never free this block mem.
	memcpy (paddr, res->ai_addr, sizeof(struct sockaddr));
	m_config.remote_server_addr.ai_addr = paddr;

	freeaddrinfo(res);
}
