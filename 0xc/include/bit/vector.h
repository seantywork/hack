#ifndef _BIT_VECTOR_H_
#define _BIT_VECTOR_H_



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int VECTOR_PushBackString(int rowc, char*** vec, char* new_el);

int VECTOR_EraseString(int rowc, char*** vec, int index);

int VECTOR_PushBackInt(int count, int** vec, int new_el);

int VECTOR_EraseInt(int count, int** vec, int index);



#endif