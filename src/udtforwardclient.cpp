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
extern const bool g_debug;
udtforwardclient::udtforwardclient() {}

udtforwardclient::~udtforwardclient() {}

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

	pthread_create(&tid, NULL, udtforwardclient_udt_epoll, NULL);

	pthread_detach(tid);
}

void udtforwardclient::udtforwardclient_initudtserver()
{
	//setnonblocking(m_udtsock);
	int events_read_write_error = UDT_EPOLL_IN|UDT_EPOLL_ERR ;
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
	//delete in udtforwardclient_socks5
	UDTSOCKET *pclisock = new UDTSOCKET;
	*pclisock = clisock;
	if (!udtforwardclient_checkclientaddr(addr))
	{
		UDT::close(clisock);
		return NULL;
	}
	pthread_create(&tid, NULL, udtforwardclient_socks5, pclisock);
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
	//char* buf = new char[bufsize];
	std::vector<unsigned char> vec;
	vec.resize(bufsize);
	//int recv_this_count = 0;

	while (true)
		{
			udtreadfds->clear();
			udtwritefds->clear();
			sysreadfds->clear();
			syswritefds->clear();
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
					vec.clear();
					//transport data from udt to target socket.
					UDTSOCKET usock = *i;
					int recved = recv_udtsock(usock, (char*)&vec[0], vec.capacity(), 0);

					int ssock = udtforwardclient_targetsocket_from_udtsocket(usock);
					if (ssock == UDTSOCKET_FAIL || ssock==0 || recved == UDTSOCKET_FAIL)
					{//remote close socket
						perror(strerror(errno));
						udtforwardclient_closesocket(usock, ssock);
						continue;
					}
					send_syssock(ssock, (char*)&vec[0], recved);
				}
			}

			for (auto i=sysreadfds->begin(); i!= sysreadfds->end(); i++)
			{
				int ssock = *i;
				int usock = access_map(m_socketmap, ssock);
				vec.clear();
				int recved = recv_syssock(ssock, (char*)&vec[0], bufsize, 0);
				if (recved <= 0)
				{
					udtforwardclient_closesocket(usock, ssock);
					continue;
				}
				send_udtsock(usock, (char*)&vec[0], recved);
			}
			//
		}
	return NULL;
}

int udtforwardclient::udtforwardclient_targetsocket_from_udtsocket(UDTSOCKET sock)
{
	for (auto i=m_socketmap.begin(); i!= m_socketmap.end(); i++)
	{
		if (i->second == sock)
			return i->first;
	}
	return UDTSOCKET_FAIL;
}

void * udtforwardclient::udtforwardclient_socks5(void *u)
{//socks5 协议协商
	UDTSOCKET *pu = (UDTSOCKET*)u;
	UDTSOCKET sock = *pu;
	delete pu;
	if (g_debug)
	{
		perror("forwardclient:new client in. try handshake.");
	}
	if (udtforwardclient_sock5_hello(sock)==-1)
	{
		UDT::close(sock);
		return NULL;
	}
	if (g_debug)
	{
		perror("forwardclient:handshake success, receive request.");
	}
	if (udtforwardclient_socks5_req(sock)==-1)
	{
		UDT::close(sock);
		return NULL;
	}
	if (g_debug)
	{
		perror("forwardclient:successfully build tunnel");
	}

return NULL;
}

int   udtforwardclient::udtforwardclient_sock5_hello(UDTSOCKET sock)
{
	int returnvalue = UDTSOCKET_FAIL;
	socks5_method_req_t req = {0};
	if (UDT::recv(sock, (char*)&req, sizeof(req), 0)<=0)
	{
		std::cout<< "UDT::recv error" <<std::endl;
		return returnvalue;
	}
	if (req.ver != SOCKS5_VERSION)
	{
		return returnvalue;
	}
	int nmethods = req.nmethods;
	//char *methods = new char[nmethods];
	std::unique_ptr<char[]> methods(new char[nmethods]);
	bool accept = false;
	UDT::recv(sock, methods.get(), nmethods,0);
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
		returnvalue = UDTSOCKET_SUCCESS;
		std::vector<unsigned char> vec;
		socks5protocol::response_hello_with_no_auth(vec);
		if (g_debug)
		{
			std::cout << "udtforwardclient hello to user:";
			output_content(vec, vec.size());
		}
		UDT::send(sock, (char*)&vec[0], vec.size(), 0);
	}
	else
	{
		returnvalue = -1;
		std::vector<unsigned char> vec;
		socks5protocol::response_hello_with_fail(vec);
		//int i = vec.size();
		if (g_debug)
		{
			std::cout << "udtforwardclient hello to user:";
			output_content(vec, vec.size());
		}
		UDT::send(sock, (char*)&vec[0], vec.size(), 0);
	}
	//delete[] methods;
	return returnvalue;

}

int   udtforwardclient::udtforwardclient_sock5_auth(UDTSOCKET sock)
{
	return UDTSOCKET_FAIL;
}

int   udtforwardclient::udtforwardclient_socks5_req(UDTSOCKET sock)
{
	//int returnvalue = -1;
	std::vector<unsigned char> vec;
	//接收request
	if (socks5protocol::recv_socks5_request(sock, vec)==-1)
	{
		UDT::close(sock);
	}
	//检查request类型是connect
	socks5_request_t *req = (socks5_request_t *)&vec[0];
	if (req->cmd==SOCKS5_CMD_CONNECT)
	{
		//连接目标地址
		int dstsock = 0;
		if ((dstsock = udtforwardclient_sock5_tryconnect(vec)) != -1)
		{
			//回报成功消息
			udtforwardclient_reply_success(sock);
			//连接成功，增加一个记录
			new autocritical(m_mutex);
			m_socketmap[dstsock] =  sock;
			int event_read_write = UDT_EPOLL_IN | UDT_EPOLL_ERR ;
			setudtnonblocking(sock);
			setsysnonblockingsend(dstsock);
			UDT::epoll_add_ssock(m_eid, dstsock, &event_read_write);
			UDT::epoll_add_usock(m_eid, sock, &event_read_write);
			return UDTSOCKET_SUCCESS;
		}
		else
		{
		//连接失败，发送失败消息，关闭连接。
			UDT::close(sock);
		}
	}
	else if (req->cmd==SOCKS5_CMD_BIND)
	{
		return UDTSOCKET_FAIL;
	}
	else if (req->cmd==SOCKS5_CMD_UDPASSOC)
	{
		return UDTSOCKET_FAIL;
	}
	return UDTSOCKET_FAIL;
}

int   udtforwardclient::udtforwardclient_sock5_tryconnect(std::vector<unsigned char> &vec)
{
	int target_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//vec是 socks5_request_t类型的数据。
	socks5_request_t *req = (socks5_request_t *)&vec[0];
	int port_offset = 0;
	if (req->atype == SOCKS5_ATTYPE_IPV4)
	{
		const int addrlen = 4;
		int addrbegin = sizeof(socks5_request_t);
		port_offset = addrlen +  sizeof(socks5_request_t) + 1;
		sockaddr_in addr = {0};
		memcpy(&addr.sin_addr, &vec[addrbegin], sizeof(struct in_addr));
		addr.sin_family = AF_INET;
		//memcpy(&addr.sin_port, &vec[port_offset], sizeof(uint16_t));//already network order.
		addr.sin_port = ntohs(vec[port_offset]);
		if (connect(target_socket, (sockaddr*)&addr, sizeof(sockaddr)) == 0)
		{
			return target_socket;
		}
		else
		{
			return UDTSOCKET_FAIL;
		}

	}
	else if (req->atype == SOCKS5_ATTYPE_DOMAIN)
	{
		//copyt domain name from vec.
		int domainbegin = sizeof(socks5_request_t);
		int domainlen = vec[domainbegin];
		//char *domain = new char[domainlen+1];
		std::vector<char> domain;
		domain.resize(domainlen+1);
		domain.clear();
		memcpy (&domain[0], &vec[domainbegin+1], domainlen);
		//dns reslove
		addrinfo ouraddrinfo = {0};
		ouraddrinfo.ai_family = AF_INET;
		ouraddrinfo.ai_socktype = SOCK_DGRAM;

		//ptarget_addrinfo content filled by getaddrinfo.
		//DON'T insert any statement between this and next statement.

		struct addrinfo* ptarget_addrinfo = nullptr;
		if (getaddrinfo((const char*)&domain[0], "http", &ouraddrinfo, &ptarget_addrinfo) != 0)
		{
			freeaddrinfo(ptarget_addrinfo);
			std::cout<<"can't resolve domain:"<<(char*)&domain[0]<<std::endl;
			return UDTSOCKET_FAIL;
		}
		std::unique_ptr<struct addrinfo *, decltype(&unique_freeaddrinfo)> utarget_addrinfo(nullptr, unique_freeaddrinfo);
		//reset port.
		port_offset = sizeof(socks5_request_t) + domainlen + 1 ;

		struct sockaddr_in *paddr = (struct sockaddr_in*)((*utarget_addrinfo)->ai_addr);
		memcpy(&paddr->sin_port, &vec[port_offset], sizeof(paddr->sin_port));
		//paddr->sin_port=ntohl(vec[port_offset]);//net order.
		//connect
		if (connect(target_socket, (struct sockaddr*)paddr, sizeof(struct sockaddr)) == 0)
		{
			//success
			//freeaddrinfo(ptarget_addrinfo);
			return target_socket;
		}
		//fail.
		std::cout<<"connect server fail :"<< (char*)&domain[0] <<std::endl;
		perror(strerror(errno));
		//freeaddrinfo(ptarget_addrinfo);
		return UDTSOCKET_FAIL;
	}
	else if (req->atype == SOCKS5_ATTYPE_IPV6)
	{
		return UDTSOCKET_FAIL;
	}
	else
	{
		return UDTSOCKET_FAIL;
	}
	return UDTSOCKET_FAIL;
}

void udtforwardclient::udtforwardclient_reply_success(UDTSOCKET sock)
{
	std::vector<unsigned char> vec;
	vec.resize(1024);
	socks5_response_t resp = {0};
	resp.ver = 0x05;
	resp.rsv = 0x00;
	resp.cmd = 0x00;//success.
	resp.atype = 0x01;//ipv4

	addrinfo ouraddrinfo = {0};
	ouraddrinfo.ai_family = AF_INET;
	ouraddrinfo.ai_socktype = SOCK_DGRAM;
	struct addrinfo *ptarget_addrinfo = NULL;
	if (getaddrinfo("localhost", "http", &ouraddrinfo, &ptarget_addrinfo) != 0)
	{

		freeaddrinfo(ptarget_addrinfo);
		return;
	}
	struct sockaddr_in* addr_in = (sockaddr_in*)ptarget_addrinfo->ai_addr;
	int addr_offset = sizeof(socks5_response_t);
	int addr_port_offset = addr_offset + sizeof(addr_in->sin_addr);
	int vec_len = addr_port_offset +  sizeof(addr_in->sin_port);

	memcpy(&vec[0], &resp, addr_offset);
	memcpy(&vec[addr_offset], &addr_in->sin_addr, sizeof(addr_in->sin_addr));
	memcpy(&vec[addr_port_offset], &addr_in->sin_port, sizeof(addr_in->sin_port));
	if (g_debug)
	{
		std::cout<< "forwardclient connect success:";
		output_content(vec, vec_len);
	}

	UDT::send(sock, (char*)&vec[0], vec_len, 0);
	freeaddrinfo(ptarget_addrinfo);
}

bool udtforwardclient::udtforwardclient_checkclientaddr(sockaddr addr)
{//check is address in the black list.
	return true;
}

void udtforwardclient::udtforwardclient_closesocket(UDTSOCKET usock, int ssock)
{
	new autocritical(m_mutex);
	if (g_debug)
	{
		std::cout<<"remove socket pair<ssock,usock>:"<< ssock << ", "<< usock << std::endl;
	}
	UDT::epoll_remove_usock(m_eid, usock);
	UDT::close(usock);

	UDT::epoll_remove_ssock(m_eid, ssock);
	close(ssock);
	m_socketmap.erase(ssock);
}
