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
#include <udt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <pthread.h>
#include <udt.h>
#include <udt>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void setudtnonblockingsend(UDTSOCKET sock);
void setsysnonblockingsend(int sock);
#endif /* UDTWRAPPER_H_ */
