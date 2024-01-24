#include "bit/d.h"


void Print2dBufferFromStream(char* source){

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


    char** td_buff = (char**)malloc(row_num * sizeof(char*));

    char* ptk = strtok(source, delim);

    int row = 0;

    while(ptk != NULL){

        int line_len = strlen(ptk);

        printf("%s\n", ptk);
        td_buff[row] = (char*)malloc(line_len * sizeof(char) + 1);

        memset(td_buff[row], 0 , line_len + 1);

        strcpy(td_buff[row], ptk);

        row++;

        ptk = strtok(NULL, delim);
                
    }

    printf("row num: %d, row: %d\n", row_num, row);

    for(int i = 0; i < row; i ++){

        printf("line %d: %s\n",i, td_buff[i]);

    }

    printf("EOF\n");

    for(int i = 0; i < row; i ++){

        free(td_buff[i]);

    }


    free(td_buff);
}