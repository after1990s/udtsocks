/*
 * udtforwardclient.h
 *
 *  Created on: Nov 9, 2015
 *      Author: administrator
 */

#ifndef UDTFORWARDCLIENT_H_
#define UDTFORWARDCLIENT_H_
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
//run outside firewall.
class udtforwardclient {
public:
	udtforwardclient();
	virtual ~udtforwardclient();

	static void   udtforwardclient_init();
	static void * udtforwardclient_accept(void *u);
	static void * udtforwardclient_udt_epoll(void *u);
	static void * udtforwardclient_socks5(void *u);//协商阶段
	static void   udtforwardclient_destory();
	//static udtforwardclient * getinstance();
private:
	static int   udtforwardclient_sock5_hello(UDTSOCKET sock);
	static int   udtforwardclient_sock5_auth(UDTSOCKET sock);
	static int   udtforwardclient_socks5_req(UDTSOCKET sock);
	static int   udtforwardclient_sock5_tryconnect(std::vector<unsigned char> &vec);
	static void setnonblocking(int sock);
private:

	//static udtforwardclient * m_pinstance;
	static pthread_mutex_t m_mutex;
	static UDTSOCKET m_udtsock;
	static std::map<int, UDTSOCKET> m_socketmap;
	static int m_eid;

};

#endif /* UDTFORWARDCLIENT_H_ */
