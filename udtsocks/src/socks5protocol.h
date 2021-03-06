/*
 * socks5protocol.h
 *
 *  Created on: Nov 13, 2015
 *      Author: administrator
 */

#ifndef SOCKS5PROTOCOL_H_
#define SOCKS5PROTOCOL_H_
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

#include "socks5.h"
#include "udtconfig.h"

//处理各种socks5协议的格式。
class socks5protocol {
public:
	socks5protocol();
	virtual ~socks5protocol();

	static void req(std::vector<unsigned char> &vec);
	static void response_req(std::vector<unsigned char> &vec);
	static void response_hello_with_no_auth(std::vector<unsigned char> &vec);
	static void response_hello_with_fail(std::vector<unsigned char> &vec);
	static int  recv_socks5_request(UDTSOCKET sock, std::vector<unsigned char> &vec);
};
void output_content(std::vector<unsigned char> &t,int);
void output_content(const char *t,int);
#endif /* SOCKS5PROTOCOL_H_ */
