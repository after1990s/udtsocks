/*
 * autocritical.h
 *
 *  Created on: Nov 6, 2015
 *      Author: administrator
 */

#ifndef AUTOCRITICAL_H_
#define AUTOCRITICAL_H_
#include <pthread.h>
class autocritical {
public:
	autocritical(pthread_mutex_t mutex);
	virtual ~autocritical();
	pthread_mutex_t m_mutex;
};

#endif /* AUTOCRITICAL_H_ */
