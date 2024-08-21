#ifndef _FRANK_UTILS_H_
#define _FRANK_UTILS_H_



#include "frank/core.h"








int read_file_to_buffer(uint8_t* buff, int max_buff_len, char* file_path);

int gen_random_bytestream(uint8_t* bytes, size_t num_bytes);

int bin2hex(uint8_t* hexarray, int arrlen, uint8_t* bytearray);











#endif