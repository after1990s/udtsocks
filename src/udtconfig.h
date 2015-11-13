/*
 * udtconfig.h
 *
 *  Created on: Nov 7, 2015
 *      Author: administrator
 */

#ifndef UDTCONFIG_H_
#define UDTCONFIG_H_
#include <string.h>

typedef struct _config{
	char domain[1024];
	int port;
	char pwd[32];
}CONFIG;
class udtconfig {
public:
	udtconfig();
	virtual ~udtconfig();
	void parseconfig();


private:
	static CONFIG m_config;
};

#endif /* UDTCONFIG_H_ */
