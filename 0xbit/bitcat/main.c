#include "bitcat/cat.h"



int main(int argc, char** argv){


    BITCAT_TARGET bc_t;

    TARGET flag = BITCAT_FlagParser(&bc_t, argc, argv);

    //TARGET flag = EARG;

    int status = 0;

    char** buff_2d;

    switch(flag){

        case EARG:

            printf("wrong argumen\n");

            return -1;

        case GETFP:

            printf("get fp\n");

            status = GetFileInto2dBuffer(&buff_2d, bc_t.file_path);

            break;
    }



    return 0;
}