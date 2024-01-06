#ifndef SERVER_ST_HEADER
#define SERVER_ST_HEADER

#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <time.h>
// s headers
#include <errno.h>
#include <arpa/inet.h>
#include <sys/time.h>
// t headers
#include <pthread.h>
#include <semaphore.h>

#endif // SERVER_ST_HEADER

#ifndef SERVER_ST_HEADER_DEF
#define SERVER_ST_HEADER_DEF

#define TRUE 1
#define FALSE 0
#define MAX_BUFF 1024 
#define MAX_CONN 80
#define PORT 8080 
#define SA struct sockaddr 

#define MAX_WORKER 2   

static struct sockaddr_in SERVADDR;
static int SERVLEN;

// to be locked
static int CLIENT_SOCKET[MAX_CONN];
static int SOCKFD;
static int MAX_SD;


static int OPT = TRUE;   


static pthread_t TID[MAX_WORKER];
static sem_t TLOCK; 

void *Worker(void *); 
void Comm();
int msleep(long);

#endif