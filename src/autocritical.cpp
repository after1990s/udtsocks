/*
 * autocritical.cpp
 *
 *  Created on: Nov 6, 2015
 *      Author: administrator
 */

#include "autocritical.h"

autocritical::autocritical(pthread_mutex_t mutex) {
	// TODO Auto-generated constructor stub
	m_mutex = mutex;
	pthread_mutex_lock(&m_mutex);
}

autocritical::~autocritical() {
	// TODO Auto-generated destructor stub
	pthread_mutex_unlock(&m_mutex);
}

