#ifndef _BIT_C_H_
#define _BIT_C_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void MaxAreaInterface();

int GetMaxAreaHeadToTail(int* height, int head, int tail);

int maxArea(int* height, int heightSize);



#define MAX_LONGEST_PALINDROMIC_SUBSTR_LENGTH 1000




void LongestPalindromicSubstrInterface();

char* longestPalindrome(char* s) ;

void PollMaxTowardsRight(char* s, int* at, int* longest);

int ExpansionWidthCounter(char*s, int start, int end, int total_len);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif