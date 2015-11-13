/*
 * udtsocksserver.cpp
 *
 *  Created on: Nov 6, 2015
 *      Author: administrator
 */

#include "udtsocksserver.h"

udtsocksserver m_pinstance=NULL;
udtsocksserver::udtsocksserver() {
	// TODO Auto-generated constructor stub
	pthread_mutex_init(&m_mutex, NULL);
}

udtsocksserver::~udtsocksserver() {
	// TODO Auto-generated destructor stub
}

static udtsocksserver &udtsocksserver::get_instance()
{
	//thread unsafe.
	if (m_pinstance==NULL)
	{
		m_pinstance = new udtsocksserver();
	}
	return *m_pinstance;
}

void udtsocksserver::udtsocksserver_init(const sockaddr *addr, int port)
{
	//inital socket
	udtsocksserver::m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//inital addr.
	int on = 1;
	if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		std::cout << "setsockopt error" << std::endl;
		return;
	}
	//bind and listen.
	bind(m_socket, addr, sizeof(sockaddr));
	listen(m_socket, 10);
	//create epoll
	m_eid = epoll_create(1);

	//create thread to accept new connect.
	pthread_create(&m_epoll_thread, NULL, udtsocksserver_accept, &m_socket);
	//detach thread
	pthread_detach(m_epoll_thread);
	return ;
}
void * udtsocksserver::udtsocksserver_accept(void *psocket)
{
	//get socket
	int socket = *(int*)psocket;
	sockaddr cliaddr = {0};
	//while true : accept
	while (true){
		unsigned int addrlen = sizeof(sockaddr);
		int clisocket = accept(socket, &cliaddr, &addrlen);
		//set socket to async.
		setnonblocking(clisocket);
		// add socket to epoll eid. add socket to socket map,should use mutex proctect.
		struct epoll_event socket_epoll_event = {0};
		socket_epoll_event.data.fd = clisocket;
		socket_epoll_event.events = EPOLLIN;
		new autocritical(m_mutex);
		m_socket_status_map.insert(std::pair<int,int>(clisocket, SOCKSHELLO));
		epoll_ctl(m_eid, EPOLL_CTL_ADD, clisocket, &socket_epoll_event);
		//create thread  epoll, if epoll thread no exits.
		if (m_epoll_thread==NULL)
		{
			pthread_create(&m_epoll_thread, NULL, udtsocksserver_epoll, &m_eid);
		}
	}//end while


	//error handle.
	return NULL;
}
void udtsocksserver::setnonblocking(int sock)
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
//this function implement socks5 proxy protocol.
void * udtsocksserver::udtsocksserver_epoll(void *peid)
{
	//while : wait epoll
	const int epoll_event_size = 20;
	epoll_event sock_epoll_events[epoll_event_size] = {0};
	std::vector<unsigned char> vec_buf;
	while (true)
	{
		memset (sock_epoll_events, 0, epoll_event_size * sizeof(epoll_event));
		//check socket status:
		int events_count = epoll_wait(m_eid, sock_epoll_events, epoll_event_size, -1);
		for (int i=0; i<events_count; i++)
		{
			int sock = sock_epoll_events[i].data.fd;
			SOCKSTATUS status = m_socket_status_map[sock];
			if (udtsocksserver_recv_all(sock, vec_buf)==0)
			{//socket closed.
				continue;
			}
			if (sock_epoll_events[i].events & EPOLLIN)
			{
				switch(m_socket_status_map[sock])
				{
				case SOCKSHELLO:
				//0 udtsocksserver_hello
					udtsocksserver_handle_hello(sock, vec_buf);
					continue;
				//1 udtsocksserver auth(not implement)
				case SOCKSAUTH:
					continue;
				//2 udtsocksserver_unconnect
				case SOCKSREQUIRE:
					udtsocksserver_handle_require(sock, vec_buf);
				//3 udtsocksserver_connect
				case SOCKSCONNECTED:
					udtsocksserver_handle_connect(sock, vec_buf);
					continue;
				default:
					udtsocksserver_handle_undef(sock, vec_buf);
					//goto end;
				}
			}
			else
			{
				goto end;
			}

		}
	}//endwhile
	//error handle
end:
	perror("udtsocksserver_epoll error");
	return NULL;
}
//read data ,return 1.
//socket closed, return 0.
int udtsocksserver::udtsocksserver_recv_all(int sock, std::vector<unsigned char> &vec)
{
	const int vec_default_size = 1024;
	int vec_size = vec_default_size;
	int already_read_count = 0;
	vec.clear();
	vec.resize(vec_size);
	while (true)
	{
		already_read_count = 0;
		vec_size = vec_default_size;
		vec.resize(vec_size);
		// read fd, return count
		int count = read(sock, &vec[already_read_count], vec_size);
		already_read_count += vec_size;
		// if count == -1
		if (count==-1)
		{
			if (errno == EAGAIN)
			{//we read all data, return 1.
				return 1;
			}
			else
			{//resize vec 2 times, continue;
				vec_size *= 2;
				vec.resize(vec_size);
				continue;
			};
			udtsocksserver_handle_close(sock, vec);
			return 0;
		}
	}//end while
	return -1;
}

int  udtsocksserver::udtsocksserver_handle_hello(int sock, std::vector<unsigned char> &vec)
{
	//check sock status
	if (m_socket_status_map[sock]!=SOCKSHELLO)
	{
		perror("udtsocksserver_handle_hello: status incorrect");
		return -1;
	}
	//if it was correct hello message
	//we don't check all methods
	if (vec[0]==0x05)
	{
		//sendback hello,with no auth
		std::vector<unsigned char> hello;
		hello.resize(2);
		hello[0]=0x05;
		hello[1]=0x00;
		write(sock, &hello[0], 2);
		//set sock status to require
		m_socket_status_map[sock] = SOCKSREQUIRE;
	}
	return 0;
}
int  udtsocksserver::udtsocksserver_handle_auth(int sock, std::vector<unsigned char> &vec)
{
	return -1;
}
int udtsocksserver::udtsocksserver_handle_require(int sock, std::vector<unsigned char> &vec)
{
	//check status.
	if (m_socket_status_map[sock]!=SOCKSREQUIRE)
	{
		perror("udtsocksserver_handle_require: status incorrect");
		return -1;
	}
	//check require class
	switch(vec[1])
	{
	//case connect
	case 1:
			//notify udtsocksclient one sock require connect
		get_udtclient().udtsocksclient_notify_connect(sock, vec);
			//if connect success , udtsocksclient  should set sock status.
		return 0;
	//case bind
	case 2:
		//not implement
		return -1;
	//case UDP ASSOCIATE
	case 3:
		//not implement
		return -1;
	}

	return 0;
}
int  udtsocksserver::udtsocksserver_handle_connect(int sock, std::vector<unsigned char> &vec)
{
	//check status.
	if (m_socket_status_map[sock]!=SOCKSCONNECTED)
	{
		perror("udtsocksserver_handle_connect: status incorrect");
				return -1;
	}
	//notify udtsocksclient send on message.
	get_udtclient().udtsocksclient_notify_send(sock, vec);
	return 0;
}
int udtsocksserver::udtsocksserver_handle_close(int sock, std::vector<unsigned char> &vec)
{
	//enter mutex
	autocritical(m_mutex);
	//remove sock in eid.
	epoll_event events={0};
	events.data.fd = sock;
	events.events = EPOLLIN;
	epoll_ctl(m_eid, EPOLL_CTL_DEL, sock, &events);
	m_socket_status_map.erase(sock);
	//close sock
	close(sock);
	//notify udtsocksclient sock close.
	get_udtclient().udtsocksclient_notify_close(sock);
	return 0;
}
udtsocksclient & udtsocksserver::get_udtclient()
{
	if (m_pudtclient==NULL)
	{
		m_pudtclient = udtsocksclient::getinstance();
	}
	return *m_pudtclient;
}
void   udtsocksserver::udtsocksserver_notify_connected(int srvsock, bool bconnect, std::vector<unsigned char> &vec)
{
	autocritical(m_mutex);
	if (bconnect)
	{
		m_socket_status_map[srvsock] = SOCKSCONNECTED;
	}

	//send vec
	write(srvsock, &vec[0], vec.size());
	//if connect failed, close socket.
	if (!bconnect)
	{
		m_socket_status_map.erase(srvsock);
		close(srvsock);
	}
}
void   udtsocksserver::udtsocksserver_notify_closed(int srvsock)
{
	autocritical(m_mutex);
	m_socket_status_map.erase(srvsock);
	close(srvsock);
}
void   udtsocksserver::udtsocksserver_notify_forward(int srvsock, std::vector<unsigned char> &vec)
{
	//send vec.
	write(srvsock, &vec[0], vec.size());
}
