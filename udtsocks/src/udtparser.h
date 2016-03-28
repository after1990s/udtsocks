/*
 * udtparser.h
 *
 *  Created on: Mar 28, 2016
 *      Author: administrator
 */

#ifndef UDTPARSER_H_
#define UDTPARSER_H_
typedef struct onlinemsg{
	int index;
	unsigned int length;
}ONLINEMSG, *PONLINEMSG;
class udtparser {
public:
	udtparser();
	virtual ~udtparser();
};

#endif /* UDTPARSER_H_ */
