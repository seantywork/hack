#include "bit/d.h"


void StreamTo2dCopyInterface(){

    char source[MAX_STREAM_LEN] = {0};

    FILE *fp = fopen("test.txt", "r");
    if (fp != NULL) {
        if (fseek(fp, 0L, SEEK_END) == 0) {
            
            long bufsize = ftell(fp);
            
            if (bufsize == -1) { 
                exit(EXIT_FAILURE);
            }

            printf("total buff len: %ld\n", bufsize);

            if (fseek(fp, 0L, SEEK_SET) != 0) { 
                exit(EXIT_FAILURE);
            }

            size_t read_len = fread(source, sizeof(char), bufsize, fp);
            if ( ferror( fp ) != 0 ) {
                fputs("error reading file", stderr);
            } else {
                source[read_len++] = '\0';
            }
        }
        fclose(fp);
    }


    Print2dBufferFromBuffer(source);



}