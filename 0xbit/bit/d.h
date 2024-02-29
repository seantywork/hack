#ifndef _BIT_D_H_
#define _BIT_D_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>



void MaxAreaInterface();

int GetMaxAreaHeadToTail(int* height, int head, int tail);

int maxArea(int* height, int heightSize);



#define MAX_LONGEST_PALINDROMIC_SUBSTR_LENGTH 1000

void LongestPalindromicSubstrInterface();

char* longestPalindrome(char* s) ;

void PollMaxTowardsRight(char* s, int* at, int* longest);

int ExpansionWidthCounter(char*s, int start, int end, int total_len);


void ReverseInt32Interface();

int reverse(int x);

int ReverseInt32WithOverflowCheck(char* x_str);

int CheckInt32StrValidity(char* a);



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


#define MAX_STREAM_LEN 1024 * 1024 
#define MAX_ROW_LEN 1024 

void StreamTo2dInterface();

void Print2dBufferFromStream(char* source);

#define MAX_ROW_NUM 128

void StreamTo2dCopyInterface();
void Print2dBufferFromBuffer(char* source);
char** Copy2dBufferFromBuffer(int rowc, char** source); 
// char** malloc -> func : okay
// char** -> func malloc : not okay
// func malloc -> char** : okay
// char* malloc -> func : okay
// char* -> func malloc : not okay
// func malloc -> char* : okay


struct StructWith2dArr{

    int name;
    char first_line_arr[MAX_CHAR_ARR_LEN]; // value
    char* first_line_ptr; // ref
    char* td_buff[MAX_ROW_NUM]; // ref
    char** dd_buff; // ref
};

void StreamTo2dStructInterface();
void Print2dBufferFromStruct(char* source);


struct VectorString{

    int size;

    char** vector;

};


void PushBackStringInterface();
void PrintPushBackStringStruct(char* source);
struct VectorString* PushBackString(struct VectorString* sv, char* new_str);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif