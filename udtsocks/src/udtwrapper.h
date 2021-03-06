/*
 * udtwrapper.h
 *
 *  Created on: Nov 25, 2015
 *      Author: administrator
 */

#ifndef UDTWRAPPER_H_
#define UDTWRAPPER_H_
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <pthread.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdexcept>

#include <udt.h>
#include <udt>
#include "socks5.h"
#include "socks5protocol.h"
void setudtnonblocking(UDTSOCKET sock);
void setsysnonblockingsend(int sock);
int send_syssock(int sock, const char * buf, int len);
int send_udtsock(UDTSOCKET sock, const char * buf, int len);
int recv_syssock(int sock, char * buf, int len, int flag);
int recv_udtsock(UDTSOCKET sock, char * buf, int len, int flag);
int access_map(const std::map<int, UDTSOCKET> &map, int k);
void unique_freeaddrinfo(struct addrinfo * & info);

#endif /* UDTWRAPPER_H_ */
