#ifndef _BIT_CAT_H_
#define _BIT_CAT_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum TARGET {

    EARG,
    GETFP,

} TARGET;


typedef struct BITCAT_TARGET {

    char* file_path;

} BITCAT_TARGET;





int GetFileInto2dBuffer(char*** buff_2d, char* file_path);

TARGET BITCAT_FlagParser(BITCAT_TARGET* bc_t, int argc, char** argv);






#endif