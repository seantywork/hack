#ifndef _FRANK_H_
#define _FRANK_H_



#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdint.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <time.h>
#include <sys/time.h>
#include <endian.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>



#define TIMEOUT_SEC 5
#define TIMEOUT_MS TIMEOUT_SEC * 1000

#define FWORD           8

#define TRUE 1
#define FALSE 0
#define MAX_BUFF FWORD * 1280 // 10KB
#define MAX_CONN 80
#define MAX_PW_LEN 4096



#define IS_BIG_ENDIAN (!*(unsigned char *)&(uint16_t){1})

#if __BIG_ENDIAN__
# define htonll(x) (x)
# define ntohll(x) (x)
#else
# define htonll(x) (((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
# define ntohll(x) (((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#endif




extern int s_sig_num;


#endif


