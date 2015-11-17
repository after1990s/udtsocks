/*
 * udtforwardclient.cpp
 *
 *  Created on: Nov 9, 2015
 *      Author: administrator
 */

#include "udtforwardclient.h"
#include "socks5.h"
pthread_mutex_t udtforwardclient::m_mutex;
UDTSOCKET udtforwardclient::m_udtsock = 0;
int udtforwardclient::m_eid = 0;
std::map<int, UDTSOCKET> udtforwardclient::m_socketmap;
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
	//pthread_create(&tid, NULL, udtforwardclient_accept, &m_udtsock);
	pthread_detach(tid);
	pthread_create(&tid, NULL, udtforwardclient_udt_epoll, NULL);
}
void udtforwardclient::udtforwardclient_initudtserver()
{
	//setnonblocking(m_udtsock);
	int events_read_write_error = 1|4|8;
	UDT::epoll_add_usock(m_eid, m_udtsock, &events_read_write_error);
	struct addrinfo hints = {0};
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo *res = NULL;
	std::string port = udtconfig::getlistenport();
	getaddrinfo(NULL, port.c_str(), &hints, &res);
	UDT::bind(m_udtsock, res->ai_addr, res->ai_addrlen);
	UDT::listen(m_udtsock, 10);

}
void * udtforwardclient::udtforwardclient_accept(void *u)
{

	UDTSOCKET sock =*(UDTSOCKET*)u;
	sockaddr addr = {0};
	int addrlen = sizeof(sockaddr);
	pthread_t tid;

	UDTSOCKET clisock  = UDT::accept(sock, &addr, &addrlen);
	if (clisock== UDT::INVALID_SOCK)
	{
		UDT::close(clisock);
		return NULL;
	}
	if (!udtforwardclient_checkclientaddr(addr))
	{
		UDT::close(clisock);
		return NULL;
	}
	pthread_create(&tid, NULL, udtforwardclient_socks5, &clisock);
	pthread_detach(tid);
	return NULL;
}
void * udtforwardclient::udtforwardclient_udt_epoll(void *u)
{
	//init udt accept socket
	udtforwardclient_initudtserver();

	std::set<UDTSOCKET>* udtreadfds = new  std::set<UDTSOCKET>();
	std::set<UDTSOCKET>* udtwritefds= new std::set<UDTSOCKET>();
	std::set<SYSSOCKET>* sysreadfds = new std::set<SYSSOCKET>();
	std::set<SYSSOCKET>* syswritefds =new  std::set<SYSSOCKET>();
	int64_t msTimeOut = -1;
	int bufsize = 1024 * 1024;
	char* buf = new char[bufsize];
	int recv_this_count = 0;

	while (true)
		{
			int res = UDT::epoll_wait(m_eid, udtreadfds, udtwritefds, msTimeOut, sysreadfds, syswritefds);
			if (res<0)
			{
				sleep(2);
				std::cout << "error:"<< UDT::getlasterror().getErrorMessage() << std::endl;
			}
			for (auto i=udtreadfds->begin(); i!=udtreadfds->end(); i++)
			{
				if (*i==m_udtsock)
				{	//new connection
					udtforwardclient_accept(&m_udtsock);
				}
				else
				{
					//data comes
					memset(buf, 0, bufsize);
					//transport data from udt to target socket.
					UDTSOCKET usock = *i;
					int recved = UDT::recv(usock, buf, bufsize, 0);
					int ssock = udtforwardclient_targetsocket_from_udtsocket(usock);
					if (ssock==-1 || ssock==0)
					{//remote close socket
						udtforwardclient_closesocket(usock, ssock);
						continue;
					}
					udtforwardclient_send_syssock(ssock, buf, recved);
				}
			}

			for (auto i=sysreadfds->begin(); i!= sysreadfds->end(); i++)
			{
				int ssock = *i;
				int usock = m_socketmap[ssock];
				memset(buf, 0, bufsize);
				int recved = recv(ssock, buf, bufsize, 0);
				if (recved == 0 || recved == -1)
				{
					udtforwardclient_closesocket(usock, ssock);
					continue;
				}
				udtforwardclient_send_udtsock(m_socketmap[ssock], buf, bufsize);
			}
			//
		}
}

void  udtforwardclient::udtforwardclient_send_udtsock(UDTSOCKET sock, const char * buf, int len)
{
	int reversed = len;
	while (reversed !=0)
	{
		int writed = UDT::send(sock, buf, reversed, 0);
		if (writed == -1)
			break;
		reversed -= writed;
	}
}

void  udtforwardclient::udtforwardclient_send_syssock(int sock, const char * buf, int len)
{
	int reversed = len;
	while (reversed !=0)
	{
		int writed = send(sock, buf, reversed, 0);
		if (writed == -1)
			break;
		reversed -= writed;
	}
}
int udtforwardclient::udtforwardclient_targetsocket_from_udtsocket(UDTSOCKET sock)
{
	for (auto i=m_socketmap.begin(); i!= m_socketmap.end(); i++)
	{
		if (i->second == sock)
			return i->first;
	}
	return -1;
}
void * udtforwardclient::udtforwardclient_socks5(void *u)
{//socks5 协议协商
	UDTSOCKET sock = *(UDTSOCKET*)u;
	if (udtforwardclient_sock5_hello(sock)==-1)
	{
		UDT::close(sock);
		return NULL;
	}
	if (udtforwardclient_socks5_req(sock)==-1)
	{
		UDT::close(sock);
		return NULL;
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
	socks5protocol::recv_socks5_request(sock, vec);
	//检查request类型是connect
	socks5_request_t *req = (socks5_request_t *)&vec[0];
	if (req->cmd==SOCKS5_CMD_CONNECT)
	{
		//连接目标地址
		int dstsock = 0;
		if ((dstsock = udtforwardclient_sock5_tryconnect(vec)) != -1)
		{
			//连接成功，增加一个记录
			m_socketmap[dstsock] =  sock;
			//将sock,dstsock加入异步列表
			setnonblocking(dstsock);
			setnonblocking(sock);
			new autocritical(m_mutex);
			int event_read_write = UDT_EPOLL_IN | UDT_EPOLL_ERR ;
			UDT::epoll_add_ssock(m_eid, dstsock, &event_read_write);
			UDT::epoll_add_usock(m_eid, sock, &event_read_write);
		}
		else
		{
		//连接失败，发送失败消息，关闭连接。
			UDT::close(sock);
		}
	}
	else if (req->cmd==SOCKS5_CMD_BIND)
	{
		return -1;
	}
	else if (req->cmd==SOCKS5_CMD_UDPASSOC)
	{
		return -1;
	}
	return -1;
}

int   udtforwardclient::udtforwardclient_sock5_tryconnect(std::vector<unsigned char> &vec)
{
	int target_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//vec是 socks5_request_t类型的数据。
	socks5_request_t *req = (socks5_request_t *)&vec[0];
	if (req->atype == SOCKS5_ATTYPE_IPV4)
	{
		int addrbegin = sizeof(socks5_request_t);
		int addrport = addrbegin +  sizeof(socks5_request_t);
		sockaddr_in addr = {0};
		memcpy(&addr.sin_addr, &vec[addrbegin], sizeof(sockaddr_in));
		addr.sin_family = AF_INET;
		addr.sin_port = vec[addrport];//already network order.

		if (connect(target_socket, (sockaddr*)&addr, sizeof(sockaddr)) == 0)
		{
			return target_socket;
		}
		else
		{
			return -1;
		}

	}
	else if (req->atype == SOCKS5_ATTYPE_DOMAIN)
	{
		//copyt domain name from vec.
		int domainbegin = sizeof(socks5_request_t);
		int domainlen = vec[domainbegin];
		char *domain = new char[domainlen+1];
		memset (domain, 0, domainlen+1);
		memcpy (domain, &vec[domainbegin], domainlen);
		//dns reslove
		addrinfo ouraddrinfo = {0};
		ouraddrinfo.ai_family = AF_INET;
		ouraddrinfo.ai_socktype = SOCK_STREAM;
		struct addrinfo *ptarget_addrinfo = NULL;
		if (getaddrinfo(domain, "http",&ouraddrinfo, &ptarget_addrinfo) != 0)
		{
			delete []domain;
			freeaddrinfo(ptarget_addrinfo);
			return -1;
		}
		delete []domain;
		//connect
		if (connect(target_socket, ptarget_addrinfo->ai_addr, ptarget_addrinfo->ai_addrlen) ==0 )
		{
			//success
			freeaddrinfo(ptarget_addrinfo);
			return target_socket;
		}
		//fail.
		freeaddrinfo(ptarget_addrinfo);
		return -1;
	}
	else if (req->atype == SOCKS5_ATTYPE_IPV6)
	{
		return -1;
	}
	else
	{
		return -1;
	}
	return -1;
}
bool udtforwardclient::udtforwardclient_checkclientaddr(sockaddr addr)
{//check is address in the black list.
	return true;
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
void udtforwardclient::udtforwardclient_closesocket(UDTSOCKET usock, int ssock)
{
	UDT::close(usock);
	UDT::epoll_remove_usock(m_eid, usock);

	UDT::epoll_remove_ssock(m_eid, ssock);
	close(ssock);
	m_socketmap.erase(ssock);
}
