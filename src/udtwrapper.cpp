/*
 * udtwrapper.cpp
 *
 *  Created on: Nov 25, 2015
 *      Author: administrator
 */

#include "udtwrapper.h"

void setudtnonblockingsend(UDTSOCKET sock)
{
	bool bfalse = false;
	UDT::setsockopt(sock, 0, UDT_SNDSYN, &bfalse, sizeof(bool));
}
void setsysnonblockingsend(int sock)
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
