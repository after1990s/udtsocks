///*
// * udtforwardclient_test.cpp
// *
// *  Created on: Nov 9, 2015
// *      Author: administrator
// */
//
//#include "udtforwardclient.h"
//#include <iostream>
//#include <unistd.h>
//#include <cstdlib>
//#include <cstring>
//#include <iostream>
//#include <map>
//#include <udt.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <sys/times.h>
//#include <sys/epoll.h>
//#include <netdb.h>
//#include <pthread.h>
//#include <udt.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <errno.h>
//#include "autocritical.h"
//#include "socks5.h"
//#include "socks5protocol.h"
//#include "udtconfig.h"
//#include "udtforwardclient.h"
//
//
//int main(void)
//{
//	udtconfig::setlistenport("8008");
//	udtforwardclient::udtforwardclient_init();
//
//	UDTSOCKET sock = UDT::socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
//	struct addrinfo hint={0}, *peer;
//	hint.ai_family = AF_INET;
//	hint.ai_socktype = SOCK_STREAM;
//	hint.ai_flags = AI_PASSIVE;
//	if (0!=getaddrinfo("localhost", "8008", &hint, &peer))
//	{
//		std::cout << "error get addrinfo : localhost" << std::endl;
//		return 0;
//	}
//	if (UDT::ERROR == UDT::connect(sock, peer->ai_addr, peer->ai_addrlen))
//	{
//		std::cout << "udt::connect error: localhost" << std::endl;
//			return 0;
//	}
//	char buf[32] = {0};
//	buf[0] = 0x05;
//	buf[1] = 0x01;
//	buf[2] = 0x00;
//	UDT::send(sock, buf, 3, 0);
//	memset(buf, 0, 32);
//	UDT::recv(sock, buf, 32, 0);
//
//	//connect
//	memset (buf, 0, 32);
//	buf[0] = 0x05;
//	buf[1] = 0x01;//connect
//	buf[2] = 0x00;//rsv;
//	buf[3] = 0x03;//domain name
//	char domain[] = "www.baidu.com";
//	int domain_len = sizeof(domain) - 1;//with out \0.
//	buf[4] = domain_len;//len(www.baidu.com)
//	memcpy(&buf[5], domain, domain_len);
//	buf[5 + domain_len] = 0x50;
//	buf[5 + domain_len + 1] = 0x00;//port
//
//	UDT::send(sock, buf, 5 + domain_len + sizeof(short), 0);
//	memset (buf, '1', 32);
//	UDT::recv(sock, buf, 32, 0);
//
//	char http_get[]="Get index.html\r\nHost:www.baidu.com\r\n\r\n";
//	int i = UDT::send(sock, http_get, sizeof(http_get), 0);
//	memset (buf, '1', 32);
//	i = UDT::recv(sock, buf, 32, 0);
//	UDT::close(sock);
//	freeaddrinfo(peer);
//	sleep(100);
//}
//
