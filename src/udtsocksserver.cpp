/*
 * udtsocksserver.cpp
 *
 *  Created on: Nov 6, 2015
 *      Author: administrator
 */

#include "udtsocksserver.h"
int udtsocksserver::m_socket;
int udtsocksserver::m_eid;
pthread_mutex_t udtsocksserver::m_mutex;
pthread_t udtsocksserver::m_epoll_thread = 0;
std::map<int,int> udtsocksserver::m_socketmap;//<socket, UDTSOCKET>

udtsocksserver::udtsocksserver() {


}

udtsocksserver::~udtsocksserver() {
	
	UDT::cleanup();
}


void udtsocksserver::udtsocksserver_init()
{
	pthread_mutex_init(&m_mutex, NULL);
	m_epoll_thread = 0;
	UDT::startup();
	//inital socket
	udtsocksserver::m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//inital addr.
	struct sockaddr_in addr ={0};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(atoi(udtconfig::getlistenport().c_str()));


	int on = 1;
	if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		std::cout << "setsockopt error" << std::endl;
		return;
	}
	//bind and listen.
	bind(m_socket, (struct sockaddr*)&addr, sizeof(addr));
	listen(m_socket, 10);
	//create epoll
	m_eid = UDT::epoll_create();
	int read_events = UDT_EPOLL_IN |UDT_EPOLL_ERR;

	UDT::epoll_add_ssock(m_eid,m_socket, &read_events);
	//create thread to accept new connect.
	pthread_create(&m_epoll_thread, NULL, udtsocksserver_epoll, &m_socket);
	pthread_detach(m_epoll_thread);
	pthread_create(&m_epoll_thread, NULL, udtsocksserver_epoll, &m_socket);
	pthread_detach(m_epoll_thread);
	pthread_create(&m_epoll_thread, NULL, udtsocksserver_epoll, &m_socket);
	pthread_detach(m_epoll_thread);
	return ;
}

//this function implement socks5 proxy protocol.
void * udtsocksserver::udtsocksserver_epoll(void *peid)
{
	//while : wait epoll
	//const int epoll_event_size = 20;

	std::set<UDTSOCKET>* udtreadfds = new  std::set<UDTSOCKET>();
	std::set<UDTSOCKET>* udtwritefds= new std::set<UDTSOCKET>();
	std::set<SYSSOCKET>* sysreadfds = new std::set<SYSSOCKET>();
	std::set<SYSSOCKET>* syswritefds =new  std::set<SYSSOCKET>();
	int64_t msTimeOut = -1;

	std::vector<unsigned char> vec_buf;
	vec_buf.resize(1024*10);
	while (true)
	{
		vec_buf.clear();
		udtreadfds->clear();
		udtwritefds->clear();
		sysreadfds->clear();
		syswritefds->clear();
		//check socket status:
		int res = UDT::epoll_wait(m_eid, udtreadfds, udtwritefds, msTimeOut, sysreadfds, syswritefds);
		if (res<0)
		{
			sleep(2);
			perror("udt epoll fail!");
			continue;
		}
		for (auto i=sysreadfds->begin(); i!=sysreadfds->end(); i++)
		{
			if (*i == m_socket)
			{//new connection
				if (g_debug){
					std::cout <<"new counnection come, try accept";
					output_content(vec_buf, 0);
				}
				udtsocksserver_accept(&m_socket);
				if (g_debug){
					std::cout <<"new counnection comes, accept successful";
					output_content(vec_buf, 0);
				}
				continue;
			}
			int ssock = *i;
			int usock = access_map(m_socketmap, ssock);
			int iread = recv(ssock, &vec_buf[0], vec_buf.capacity(), 0);
			if (iread<=0)
			{
				//socket closed.
				udtsocksserver_closesocket(usock, ssock);
				continue;
			}

			if (g_debug){
				std::cout <<"recv from user:";
				output_content(vec_buf, iread);
			}
			UDT::send (usock, (char*)&vec_buf[0], iread, 0);//socks5 request
		}
		for (auto i=udtreadfds->begin(); i!= udtreadfds->end(); i++)
		{
			int ssock = udtsocksserver_sourcesock_from_udt(*i);
			if (ssock==-1)
			{
				udtsocksserver_closesocket(*i, 0);
				continue;
			}
			int usock = *i;
			vec_buf.resize(1024);
			int recved = UDT::recv(usock, (char*)&vec_buf[0], vec_buf.capacity(), 0);
			if (recved <= 0)
			{//socket close
				perror(UDT::getlasterror_desc());
				udtsocksserver_closesocket(usock, ssock);
				continue;
			}
			if(g_debug)
			{
				std::cout <<"recv from  remote:";
				output_content(vec_buf, recved);
			}
			send(ssock, (void*)&vec_buf[0], recved, 0);//socks5 response
		}
	}//endwhile
	//error handle

	perror("udtsocksserver_epoll error");
	return NULL;
}
void udtsocksserver::udtsocksserver_closesocket(UDTSOCKET usock, int ssock)
{

	UDTSOCKET u = access_map(m_socketmap, ssock);
	if (u != usock)
	{
		perror("Warning:socket pair does not pair.");
		//pause();
	}
	if (usock!=0){
		UDT::epoll_remove_usock(m_eid, usock);
		UDT::close(usock);
	}
	if (ssock!=0)
	{
		UDT::epoll_remove_ssock(m_eid, ssock);
		close(ssock);
	}
	m_socketmap.erase(ssock);
}
int udtsocksserver::udtsocksserver_sourcesock_from_udt(UDTSOCKET usock)
{
	for (auto i=m_socketmap.begin(); i != m_socketmap.end(); i++)
	{
		if (i->second == usock)
		{
			return i->first;
		}
	}

	return UDTSOCKET_FAIL;
}
void * udtsocksserver::udtsocksserver_accept(void *psocket)
{
	//get ssocket
	int socket = *(int*)psocket;
	sockaddr cliaddr = {0};

	{
		unsigned int addrlen = sizeof(sockaddr);
		int clisocket = accept(socket, &cliaddr, &addrlen);
		//set socket to async.
		setsysnonblockingsend(clisocket);
		// add socket to epoll eid. add socket to socket map,should use mutex proctect.
		UDTSOCKET newclient = connectserver();
		if (newclient == UDTSOCKET_FAIL)
		{
			close(clisocket);
			perror("connect server failed :");
			//todo: rely fail msg to user.
			return NULL;
		}
		new autocritical(m_mutex);
		m_socketmap.insert(std::pair<int,int>(clisocket, newclient));
		//bool f = false;
		int event_read = UDT_EPOLL_IN;
		UDT::epoll_add_ssock(m_eid, clisocket, &event_read);
		UDT::epoll_add_usock(m_eid, newclient, &event_read);

	}//end while


	//error handle.
	return NULL;
}



UDTSOCKET udtsocksserver::connectserver(void)
{
	UDTSOCKET sock = UDT::socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
	struct sockaddr addr =  udtconfig::getserveraddr();
	struct sockaddr dst = addr;
	if (g_debug){
		std::cout <<"try connect server";
		output_content(NULL, 0);
	}
	if (UDT::connect(sock, (sockaddr*)&dst, sizeof(dst)) != 0)
	{
		if (g_debug){
			std::cout <<"connect server failed";
			perror(UDT::getlasterror_desc());
			output_content(NULL, 0);
		}
		perror( UDT::getlasterror_desc());
		UDT::close(sock);
		return UDTSOCKET_FAIL;
	}
	if (g_debug){
		std::cout <<"end connect server";
		output_content(NULL, 0);
	}
	setudtnonblockingsend(sock);
	return sock;
}
////read data ,return 1.
////socket closed, return UDTSOCKET_SUCCESS.
//int udtsocksserver::udtsocksserver_recv_all(int sock, std::vector<unsigned char> &vec)
//{
//	const int vec_default_size = 1024;
//	int vec_size = vec_default_size;
//	int already_read_count = 0;
//	vec.clear();
//	vec.resize(vec_size);
//	while (true)
//	{
//		already_read_count = 0;
//		vec_size = vec_default_size;
//		vec.resize(vec_size);
//		// read fd, return count
//		int count = read(sock, &vec[already_read_count], vec_size);
//		already_read_count += vec_size;
//		// if count == -1
//		if (count==-1)
//		{
//			if (errno == EAGAIN)
//			{//we read all data, return 1.
//				return 1;
//			}
//			else
//			{//resize vec 2 times, continue;
//				vec_size *= 2;
//				vec.resize(vec_size);
//				continue;
//			};
//			udtsocksserver_handle_close(sock, vec);
//			return UDTSOCKET_SUCCESS;
//		}
//	}//end while
//	return UDTSOCKET_FAIL;
//}
//
//int  udtsocksserver::udtsocksserver_handle_hello(int sock, std::vector<unsigned char> &vec)
//{
//	//check sock status
//	if (m_socket_status_map[sock]!=SOCKSHELLO)
//	{
//		perror("udtsocksserver_handle_hello: status incorrect");
//		return UDTSOCKET_FAIL;
//	}
//	//if it was correct hello message
//	//we don't check all methods
//	if (vec[0]==0x05)
//	{
//		//sendback hello,with no auth
//		std::vector<unsigned char> hello;
//		hello.resize(2);
//		hello[0]=0x05;
//		hello[1]=0x00;
//		write(sock, &hello[0], 2);
//		//set sock status to require
//		m_socket_status_map[sock] = SOCKSREQUIRE;
//	}
//	return UDTSOCKET_SUCCESS;
//}
//int  udtsocksserver::udtsocksserver_handle_auth(int sock, std::vector<unsigned char> &vec)
//{
//	return UDTSOCKET_FAIL;
//}
//int udtsocksserver::udtsocksserver_handle_require(int sock, std::vector<unsigned char> &vec)
//{
//	//check status.
//	if (m_socket_status_map[sock]!=SOCKSREQUIRE)
//	{
//		perror("udtsocksserver_handle_require: status incorrect");
//		return UDTSOCKET_FAIL;
//	}
//	//check require class
//	switch(vec[1])
//	{
//	//case connect
//	case 1:
//			//notify udtsocksclient one sock require connect
//		get_udtclient().udtsocksclient_notify_connect(sock, vec);
//			//if connect success , udtsocksclient  should set sock status.
//		return UDTSOCKET_SUCCESS;
//	//case bind
//	case 2:
//		//not implement
//		return UDTSOCKET_FAIL;
//	//case UDP ASSOCIATE
//	case 3:
//		//not implement
//		return UDTSOCKET_FAIL;
//	}
//
//	return UDTSOCKET_SUCCESS;
//}
//int  udtsocksserver::udtsocksserver_handle_connect(int sock, std::vector<unsigned char> &vec)
//{
//	//check status.
//	if (m_socket_status_map[sock]!=SOCKSCONNECTED)
//	{
//		perror("udtsocksserver_handle_connect: status incorrect");
//				return UDTSOCKET_FAIL;
//	}
//	//notify udtsocksclient send on message.
//	get_udtclient().udtsocksclient_notify_send(sock, vec);
//	return UDTSOCKET_SUCCESS;
//}
//int udtsocksserver::udtsocksserver_handle_close(int sock, std::vector<unsigned char> &vec)
//{
//	//enter mutex
//	autocritical(m_mutex);
//	//remove sock in eid.
//	epoll_event events={0};
//	events.data.fd = sock;
//	events.events = EPOLLIN;
//	epoll_ctl(m_eid, EPOLL_CTL_DEL, sock, &events);
//	m_socket_status_map.erase(sock);
//	//close sock
//	close(sock);
//	//notify udtsocksclient sock close.
//	get_udtclient().udtsocksclient_notify_close(sock);
//	return UDTSOCKET_SUCCESS;
//}
//udtsocksclient & udtsocksserver::get_udtclient()
//{
//	if (m_pudtclient==NULL)
//	{
//		m_pudtclient = udtsocksclient::getinstance();
//	}
//	return *m_pudtclient;
//}
//void   udtsocksserver::udtsocksserver_notify_connected(int srvsock, bool bconnect, std::vector<unsigned char> &vec)
//{
//	autocritical(m_mutex);
//	if (bconnect)
//	{
//		m_socket_status_map[srvsock] = SOCKSCONNECTED;
//	}
//
//	//send vec
//	write(srvsock, &vec[0], vec.size());
//	//if connect failed, close socket.
//	if (!bconnect)
//	{
//		m_socket_status_map.erase(srvsock);
//		close(srvsock);
//	}
//}
//void   udtsocksserver::udtsocksserver_notify_closed(int srvsock)
//{
//	autocritical(m_mutex);
//	m_socket_status_map.erase(srvsock);
//	close(srvsock);
//}
//void   udtsocksserver::udtsocksserver_notify_forward(int srvsock, std::vector<unsigned char> &vec)
//{
//	//send vec.
//	write(srvsock, &vec[0], vec.size());
//}
