/*
 * udtforwardclient.cpp
 *
 *  Created on: Nov 9, 2015
 *      Author: administrator
 */

#include "udtforwardclient.h"
#include "socks5.h"
pthread_mutex_t udtforwardclient::m_mutex;
UDTSOCKET udtforwardclient::m_udtsock;
udtforwardclient::udtforwardclient() {


}

udtforwardclient::~udtforwardclient() {


}

void   udtforwardclient::udtforwardclient_destory()
{
	pthread_mutex_destroy(&m_mutex);
	UDT::cleanup();
}
void   udtforwardclient::udtforwardclient_init()
{
	UDT::startup();
	m_eid = UDT::epoll_create();
	pthread_mutex_init(&m_mutex, NULL);
	m_udtsock = UDT::socket(AF_INET,SOCK_STREAM, IPPROTO_UDP);
	pthread_t tid;
	pthread_create(&tid, NULL, udtforwardclient_accept, &m_udtsock);
	pthread_detach(tid);
	pthread_create(&tid, NULL, udtforwardclient_udt_epoll, NULL);
}
void * udtforwardclient::udtforwardclient_accept(void *u)
{

	UDTSOCKET sock =*(UDTSOCKET*)u;
	sockaddr addr = {0};
	int addrlen = sizeof(sockaddr);
	pthread_t tid;
	while (true)
	{
		UDTSOCKET sock  = UDT::accept(sock, &addr, &addrlen);
		if (sock== UDT::INVALID_SOCK)
			break;
		pthread_create(&tid, NULL, udtforwardclient_socks5, &sock);
		pthread_detach(tid);
	}
	//error handle;
	perror("udtforwardclient_accept fail");
	return NULL;
}
void * udtforwardclient::udtforwardclient_udt_epoll(void *u)
{
	std::set<UDTSOCKET>* readfds = new  std::set<UDTSOCKET>();
	std::set<UDTSOCKET>* writefds= new std::set<UDTSOCKET>();

	int64_t msTimeOut = -1;
	int bufsize = 1024 * 1024;
	char* buf = new char[bufsize];
	int recv_this_count = 0;

	while (true)
		{
			int res = UDT::epoll_wait(m_eid, readfds, writefds, msTimeOut, NULL, NULL);
			if (res<0)
			{
				sleep(2);
				std::cout << "error:"<< UDT::getlasterror().getErrorMessage() << std::endl;
			}
			//
		}
}

void * udtforwardclient::udtforwardclient_socks5(void *u)
{//socks5 协议协商
	UDTSOCKET sock = *(UDTSOCKET*)u;
	if (udtforwardclient_sock5_hello(sock)==-1)
	{
		UDT::close(sock);
	}
	if (udtforwardclient_socks5_req(sock)==-1)
	{
		UDT::close(sock);
	}


return NULL;
}

int   udtforwardclient::udtforwardclient_sock5_hello(UDTSOCKET sock)
{
	int returnvalue = -1;
	socks5_method_req_t req = {0};
	UDT::recv(sock, (char*)&req, sizeof(req), 0);
	if (req.ver != SOCKS5_VERSION)
	{
		return returnvalue;
	}
	int nmethods = req.nmethods;
	char *methods = new char[nmethods];
	bool accept = false;
	UDT::recv(sock, methods, nmethods,0);
	for (int i=0; i < nmethods; i++)
	{
		if (methods[i] == 0)
		{
			accept = true;
			break;
		}
	}
	if (accept)
	{
		returnvalue = 0;
		std::vector<unsigned char> vec;
		socks5protocol::response_hello_with_no_auth(vec);
		UDT::send(sock, (char*)&vec[0], vec.size(), 0);
	}
	else
	{
		returnvalue = -1;
		std::vector<unsigned char> vec;
		socks5protocol::response_hello_with_fail(vec);
		UDT::send(sock, (char*)&vec[0], vec.size(), 0);
	}
	delete[] methods;
	return returnvalue;

}
int   udtforwardclient::udtforwardclient_sock5_auth(UDTSOCKET sock)
{
	return -1;
}
int   udtforwardclient::udtforwardclient_socks5_req(UDTSOCKET sock)
{
	int returnvalue = -1;
	std::vector<unsigned char> vec;
	//接收request
	socks5protocol::recv_socks5_request(vec);
	//检查request类型是connect
	socks5_request_t *req = &vec[0];
	if (req->cmd==SOCKS5_CMD_CONNECT)
	{
		//连接目标地址
		int dstsock = 0;
		if ((dstsock = udtforwardclient_sock5_tryconnect(vec))!=-1)
		{
			//连接成功，增加一个记录
			m_socketmap[dstsock] =  sock;
			//将sock,dstsock加入异步列表
			setnonblocking(dstsock);
			setnonblocking(sock);
			autocritical(m_mutex);
			UDT::epoll_add_ssock(m_eid, )
		}
		else
		{
		//连接失败，发送失败消息，关闭连接。
			UDT::close(sock);
		}
	}
	else if (req->cmd==SOCKS5_CMD_BIND)
	{

	}
	else if (req->cmd==SOCKS5_CMD_UDPASSOC)
	{

	}




	return -1;
}

int   udtforwardclient::udtforwardclient_sock5_tryconnect(std::vector<unsigned char> &vec)
{
	//vec是 socks5_request_t类型的数据。
	return -1;
}

void  udtforwardclient::setnonblocking(int sock)
{
    int opts;
    opts=fcntl(sock,F_GETFL);
    if(opts<0)
    {
        perror("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts)<0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}
