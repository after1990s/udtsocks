/*
 * autocritical.cpp
 *
 *  Created on: Nov 6, 2015
 *      Author: administrator
 */

#include "autocritical.h"

autocritical::autocritical(pthread_mutex_t mutex) {

	m_mutex = mutex;
	pthread_mutex_lock(&m_mutex);
}

autocritical::~autocritical() {

	pthread_mutex_unlock(&m_mutex);
}

