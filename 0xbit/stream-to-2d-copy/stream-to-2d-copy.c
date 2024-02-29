#include "bit/d.h"




void Print2dBufferFromBuffer(char* source){

    int row_num = 0;

    char* delim = "\n";

    char* s_tmp = source;

    while(s_tmp != NULL)                                                                                                    
    {                                                                                                                       
        s_tmp = strchr(s_tmp, '\n');                                                                                                           
        if( s_tmp ) {
            s_tmp++;
            row_num++;
        }                                                                                                
    }

    printf("row_num: %d\n", row_num);


    char* td_buff[MAX_ROW_NUM];

    char* ptk = strtok(source, delim);

    int row = 0;

    while(ptk != NULL){

        int line_len= 0;

        printf("[ READ %d ] %s\n", row, ptk);

        line_len = strlen(ptk);

        td_buff[row] = (char*)malloc(line_len * sizeof(char) + 1);

        memset(td_buff[row], 0 , line_len + 1);

        strcpy(td_buff[row], ptk);

        row++;

        ptk = strtok(NULL, delim);
                
    }

    char** dd_buff;

    dd_buff = Copy2dBufferFromBuffer(row, td_buff);

    printf("row num: %d, row: %d\n", row_num, row);

    for(int i = 0; i < row; i ++){

        printf("[ LINE %d ]: %s\n",i, dd_buff[i]);

    }

    printf("[ EOF ]\n");

    for(int i = 0; i < row; i ++){

        free(td_buff[i]);
        free(dd_buff[i]);

    }

    free(dd_buff);


}




char** Copy2dBufferFromBuffer(int rowc, char** source){

    char** dest = (char**)malloc(rowc * sizeof(char*));

    for(int i = 0; i < rowc; i ++){

        int line_len = strlen(source[i]);

        dest[i] = (char*)malloc(line_len * sizeof(char) + 1);

        memset(dest[i], 0 , line_len + 1);

        strcpy(dest[i], source[i]);

        printf("[ COPIED %d ] %s\n", i, dest[i]);
    }

    return dest;


}
