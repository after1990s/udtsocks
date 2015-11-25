/*
 * udtconfig.h
 *
 *  Created on: Nov 7, 2015
 *      Author: administrator
 */

#ifndef UDTCONFIG_H_
#define UDTCONFIG_H_
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <udt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <pthread.h>
#include <udt.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "autocritical.h"
#include "socks5.h"
typedef struct _config{
	std::string domain;
	std::string listen_port;//
	std::string pwd;
	struct addrinfo remote_server_addr;//server addr.
}CONFIG;
class udtconfig {
public:
	udtconfig();
	virtual ~udtconfig();

	static void parseconfig();
	static struct sockaddr getserveraddr();
	static std::string  getlistenport();

	static void setlistenport(char *port);
	static void setserveraddr(char *addr, char* port);

private:
	static CONFIG m_config;
};

#endif /* UDTCONFIG_H_ */
