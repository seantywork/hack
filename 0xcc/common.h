#ifndef _CC_COMMON_H_
#define _CC_COMMON_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



char* readline();

void normalize_line(char* line);

char* ltrim(char* str);

char* rtrim(char* str);

char** split_string(char* str);

char** split_string2(char* str, int* len);

int parse_int(char* str);

#ifdef __cplusplus
}
#endif
#endif
