#ifndef _BIT_D_H_
#define _BIT_D_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_MEMBER_ARR_LEN 1024


void StructCopyThreadInterface();

void* StructCopyThreadForSarr (void* targ);

void* StructCopyThreadForSptr (void* targ);

struct StructWithArr{

    int name;
    char comment[MAX_MEMBER_ARR_LEN];
};


struct StructWithPtr{

    int name;
    char* comment;

};



#define MAX_CHAR_ARR_LEN 1024

void ArrAsArgInterface();

void ReceiveCharArr(char arg[MAX_CHAR_ARR_LEN]);

void ReceiveCharPtr(char* arg);





#ifdef __cplusplus
}
#endif // __cplusplus


#endif