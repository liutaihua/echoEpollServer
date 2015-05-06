
#ifndef SERVER_COMMON_H
#define SERVER_COMMON_H

enum TimeVariables
{
	TIME_SECOND = 1,
	TIME_MINUTE = TIME_SECOND * 60,
	TIME_HOUR   = TIME_MINUTE * 60,
	TIME_DAY	= TIME_HOUR * 24,
	TIME_MONTH	= TIME_DAY * 30,
	TIME_YEAR	= TIME_MONTH * 12,
};

enum MsTimeVariables
{
	MSTIME_SECOND = 1000,
	MSTIME_MINUTE = MSTIME_SECOND * 60,
	MSTIME_HOUR   = MSTIME_MINUTE * 60,
	MSTIME_DAY	= MSTIME_HOUR * 24,
};


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include "Singleton.h"

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
 #include <winsock2.h>
 #include <ws2tcpip.h>
#endif
#include  <stddef.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>


#ifdef __linux__
 #define LINUX
#endif


typedef unsigned char		byte;

typedef unsigned char		byte;

typedef unsigned char		uint8;

typedef signed short			int16;

typedef unsigned short		uint16;

typedef signed int			int32;

typedef unsigned int			uint32;

typedef signed long long		int64;

typedef unsigned long long	uint64;

#define SOCKET int



#ifdef LINUX
  #include <sys/epoll.h>
  #define CONFIG_USE_EPOLL
#else
  #include <sys/event.h>
  #define CONFIG_USE_KQUEUE
#endif


inline static unsigned int MakeIP(const char * str)
{
	unsigned int bytes[4];
	unsigned int res;
	if( sscanf(str, "%u.%u.%u.%u", &bytes[0], &bytes[1], &bytes[2], &bytes[3]) != 4 )
		return 0;

	res = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
	return res;
}


#endif
