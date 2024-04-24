#ifndef _NCAT_H_
#define _NCAT_H_

#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdint.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#define MAX_LOAD_BUFF 1024 * 1024 * 10
#define MAX_TMP_BUFF 1024 * 1024 
#define MAX_READ_BUFF 1024 * 1024
#define MAX_WRITE_BUFF MAX_READ_BUFF

typedef struct NCAT_OPTIONS {

    int mode_client;
    int mode_listen;
    int _client_sock_ready;
    int _client_sockfd;
    pthread_mutex_t _lock;
    char* host;
    char* port;

} NCAT_OPTIONS;


extern int ncat_argc;
extern char** ncat_argv;
extern NCAT_OPTIONS NCATOPTS;
extern int ncat_connect;
extern int ncat_listen;
extern int serve_content_exists;
extern char serve_content[MAX_LOAD_BUFF];
extern int serve_content_ptr;

void NCAT_keyboard_interrupt();

int NCAT_parse_args(int argc, char** argv);


void NCAT_free();


int NCAT_runner();


int NCAT_client();


int NCAT_listen_and_serve();



void* NCAT_get_thread();



void msleep(long ms);

#endif