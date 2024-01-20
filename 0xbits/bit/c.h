#ifndef _BIT_C_H_
#define _BIT_C_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void MaxAreaInterface();

int GetMaxAreaPerHead(int* height, int head, int tail);

int maxArea(int* height, int heightSize);



#ifdef __cplusplus
}
#endif // __cplusplus

#endif