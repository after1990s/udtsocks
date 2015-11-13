/*
 * udtsocksserver.h
 *
 *  Created on: Nov 6, 2015
 *      Author: administrator
 */

#ifndef UDTSOCKSSERVER_H_
#define UDTSOCKSSERVER_H_
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
#include <udtsocksclient.h>
#include "autocritical.h"
//implement socks5 proxy server. not implement auth.
//only accept method CONNECT.
//socks5 proxy not handle connect status.
//only recv pkt and return.
enum SOCKSTATUS{ SOCKSHELLO=0, SOCKSAUTH, SOCKSREQUIRE, SOCKSCONNECTED};

//communicate with costume apps like firefox.
class udtsocksserver {
private:
	udtsocksserver();
public:
	virtual ~udtsocksserver();

public:
	void   udtsocksserver_init(const sockaddr *addr, int port);
	void * udtsocksserver_accept(void *psocket);
	void * udtsocksserver_epoll(void *peid);

	//all notify_* functions called by udtsocksclient.
	void   udtsocksserver_notify_connected(int srvsock, bool bconnect, std::vector<unsigned char> &vec);
	void   udtsocksserver_notify_closed(int srvsock);
	void   udtsocksserver_notify_forward(int srvsock, std::vector<unsigned char> &vec);
	static udtsocksserver & get_instance();
private:
	int m_socket;
	int m_eid;
	pthread_t m_epoll_thread;
	std::map<int,int> m_socket_status_map;//<socket, status>;//status:0-hello->2
	std::map<int,int> m_socket_pair;//<socket, UDTSOCKET>
	void setnonblocking(int sock);
	int udtsocksserver_handle_hello(int sock, std::vector<unsigned char> &vec);
	int udtsocksserver_handle_auth(int sock, std::vector<unsigned char> &vec);
	int udtsocksserver_handle_require(int sock, std::vector<unsigned char> &vec);
	int udtsocksserver_handle_connect(int sock, std::vector<unsigned char> &vec);
	int udtsocksserver_handle_close(int sock, std::vector<unsigned char> &vec);
	int udtsocksserver_handle_undef(int sock, std::vector<unsigned char> &vec);
	int udtsocksserver_recv_all(int sock, std::vector<unsigned char> &vec);
	//handle message from udtsocksclient.
	int udtsocksserver_handle_proxy_recv(int sock, std::vector<unsigned char> &vec);

private:
	udtsocksserver * m_pinstance;
	pthread_mutex_t m_mutex;
};

#endif /* UDTSOCKSSERVER_H_ */
