#include "frank/utils.h"




int read_file_to_buffer(uint8_t* buff, int max_buff_len, char* file_path){


    int valread = 0 ;

    int c;

    FILE* fp;

    fp = fopen(file_path, "r");

    while(1) {

        c = fgetc(fp);
        if( feof(fp) ) {
            break;
        }


        buff[valread] = c;

        valread += 1;

        if(valread > max_buff_len){

            return -10;
        }
    
   }

    return valread;
}



int gen_random_bytestream(uint8_t* bytes, size_t num_bytes){
  

    if(num_bytes > MAX_PW_LEN){

        return -1;

    }


    size_t i;

    for (i = 0; i < num_bytes; i++){

        bytes[i] = rand();

    }

    return 0;

}

int bin2hex(uint8_t* hexarray, int arrlen, uint8_t* bytearray){

    int hexlen = 2;

    int outstrlen = hexlen * arrlen + 1;

    if (outstrlen > MAX_PW_LEN){

        return -1;
    }

    memset(hexarray, 0, outstrlen * sizeof(char));

    unsigned char* ptr = hexarray;

    for(int i = 0 ; i < arrlen; i++){

        sprintf(ptr + 2 * i, "%02X", bytearray[i]);
    }

    return 0;
    
}