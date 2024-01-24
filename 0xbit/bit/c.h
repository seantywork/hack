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

int IterateFromMiddle(char* s);

int ExpansionCounter(char*s, int at, int head, int tail, int odd);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif