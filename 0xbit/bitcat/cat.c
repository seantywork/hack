#include "bitcat/cat.h"



TARGET BITCAT_FlagParser(BITCAT_TARGET* bc_t, int argc, char** argv){

    int status = 0;

    if(argc != 2){

        return EARG;

    }


    int file_path_len = strlen(argv[1]) + 1;



    bc_t->file_path = (char*)malloc(file_path_len * sizeof(char));

    memset(bc_t->file_path, 0, file_path_len * sizeof(char));

    strcpy(bc_t->file_path, argv[1]);


    return GETFP;
}



int GetFileInto2dBuffer(char*** buff_2d, char* file_path){



    int valread = 0;





    return valread;
}
