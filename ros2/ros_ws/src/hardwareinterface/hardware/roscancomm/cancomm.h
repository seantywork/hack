
#ifndef CO_SOCK_H
#define CO_SOCK_H

#include <pthread.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_CAN_DEV_NAME 10

#define MAX_CAN_CMD_IN 1000

#define MAX_CAN_CMD_OUT 1000


extern pthread_t SOCK_PTID;

extern FILE* TFP;

extern char CAN_DEV_NAME[MAX_CAN_DEV_NAME];

extern int CAN_NODE_ID;

extern int SOCKFD;

int InitCanComm(char[MAX_CAN_DEV_NAME], uint8_t );

int InitCmdGateway();


int CmdGatewayASCII(char[MAX_CAN_CMD_IN] , char[MAX_CAN_CMD_OUT]);

int CloseCanCommAll();


void* cancomm_listener (void* );

int strncpy_process(char[MAX_CAN_CMD_OUT], char[MAX_CAN_CMD_OUT]);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
