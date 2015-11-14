/*
 * socks5protocol.h
 *
 *  Created on: Nov 13, 2015
 *      Author: administrator
 */

#ifndef SOCKS5PROTOCOL_H_
#define SOCKS5PROTOCOL_H_
#include <vector>
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

#endif /* SOCKS5PROTOCOL_H_ */
