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
//void * test_ipv4(void* u)//ipv4 test
//{
//		UDTSOCKET sock = UDT::socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
//		struct addrinfo hint={0}, *peer;
//		hint.ai_family = AF_INET;
//		hint.ai_socktype = SOCK_STREAM;
//		hint.ai_flags = AI_PASSIVE;
//		if (0!=getaddrinfo("localhost", "8008", &hint, &peer))
//		{
//			std::cout << "error get addrinfo : localhost" << std::endl;
//			return UDTSOCKET_SUCCESS;
//		}
//		if (UDT::ERROR == UDT::connect(sock, peer->ai_addr, peer->ai_addrlen))
//		{
//			std::cout << "udt::connect error: localhost" << std::endl;
//				return UDTSOCKET_SUCCESS;
//		}
//		char buf[32] = {0};
//		buf[0] = 0x05;
//		buf[1] = 0x01;
//		buf[2] = 0x00;
//		UDT::send(sock, buf, 3, 0);
//		memset(buf, 0, 32);
//		UDT::recv(sock, buf, 32, 0);
//
//		//connect
//		memset (buf, 0, 32);
//		buf[0] = 0x05;
//		buf[1] = 0x01;
//		buf[2] = 0x00;//rsv
//		buf[3] = 0x01;//
//		//ipv4 103.235.46.39
//		buf[4] = 0x67;
//		buf[5] = 0xeb;
//		buf[6] = 0x2e;
//		buf[7] = 0x27;
//		//port:80
//		buf[8] = 0x00;
//		buf[9] = 0x50;
//		UDT::send(sock, buf, 10, 0);
//		memset (buf, 1, 32);
//		UDT::recv(sock, buf, 32, 0);
//
//		char http_get[]=
//				"GET index.html HTTP/1.1\r\n"
//				"Host: www.baidu.com\r\n"
//				"Accept: text/html\r\n"
//				"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
//				"\r\n";
//		int i = UDT::send(sock, http_get, sizeof(http_get), 0);
//		memset (buf, '1', 32);
//		i = UDT::recv(sock, buf, 32, 0);
//		freeaddrinfo(peer);
//		return NULL;
//}
//void* domain_test(void*u)//domain test
//{
//
//	UDTSOCKET sock = UDT::socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
//	struct addrinfo hint={0}, *peer;
//	hint.ai_family = AF_INET;
//	hint.ai_socktype = SOCK_STREAM;
//	hint.ai_flags = AI_PASSIVE;
//	if (0!=getaddrinfo("localhost", "8008", &hint, &peer))
//	{
//		std::cout << "error get addrinfo : localhost" << std::endl;
//		return UDTSOCKET_SUCCESS;
//	}
//	if (UDT::ERROR == UDT::connect(sock, peer->ai_addr, peer->ai_addrlen))
//	{
//		std::cout << "udt::connect error: localhost" << std::endl;
//			return UDTSOCKET_SUCCESS;
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
//	buf[5 + domain_len] = 0x00;
//	buf[5 + domain_len + 1] = 0x50;//port
//
//	UDT::send(sock, buf, 5 + domain_len + sizeof(short), 0);
//	memset (buf, '1', 32);
//	UDT::recv(sock, buf, 32, 0);
//
//	char http_get[]=
//			"GET index.html HTTP/1.1\r\n"
//			"Host: www.baidu.com\r\n"
//			"Accept: text/html\r\n"
//			"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
//			"\r\n";
//	int i = UDT::send(sock, http_get, sizeof(http_get), 0);
//	memset (buf, '1', 32);
//	i = UDT::recv(sock, buf, 32, 0);
//	if (i<0)
//	{
//		asm("int $3");
//	}
//	freeaddrinfo(peer);
//	static int count = 0;
//	if (count != 11)
//	{
//		count++;
//	}
//	else
//	{
//		asm("int $3");
//	}
//	return NULL;
//}
//
//
//int main(void)
//{
//	udtconfig::setlistenport("8008");
//	udtforwardclient::udtforwardclient_init();
////	for (int i=0; i<12; i++)
////	{
////		pthread_t tid = 0;
////		pthread_create(&tid, NULL, domain_test, NULL);
////		pthread_detach(tid);
////
////	}
//	domain_test(NULL);
//	pause();
//
////	for (int i=0; i<10; i++)
////	{
////		pthread_t tid = 0;
////		pthread_create(&tid, NULL, test_ipv4, NULL);
////		pthread_detach(tid);
////
////	}
////	sleep(1);
////	for (int i=0; i<20; i++)
////	{
////		pthread_t tid = 0;
////		pthread_create(&tid, NULL, test_ipv4, NULL);
////		pthread_detach(tid);
////
////	}
////	pause();
//}
