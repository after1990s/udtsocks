/*
 * udtconfig.cpp
 *
 *  Created on: Nov 7, 2015
 *      Author: administrator
 */

#include "udtconfig.h"
CONFIG udtconfig::m_config;
udtconfig::udtconfig() {
	// TODO Auto-generated constructor stub

}

udtconfig::~udtconfig() {
	// TODO Auto-generated destructor stub
}

void udtconfig::parseconfig()
{
	//demon
	memset(&udtconfig::m_config, 0, sizeof(m_config));
	strncpy (udtconfig::m_config.domain,"127.0.0.1", 11);
	udtconfig::m_config.port = 9010;
	return ;
}
