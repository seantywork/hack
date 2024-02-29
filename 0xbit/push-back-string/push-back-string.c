#include "bit/d.h"




void PrintPushBackStringStruct(char* source){

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



    struct VectorString* sv = (struct VectorString*)malloc(sizeof(struct VectorString));
 
    sv->vector = (char**)malloc(row_num * sizeof(char*));

    sv->size = 0;

    char* ptk = strtok(source, delim);

    int row = 0;

    printf("row num: %d\n", row_num);

    while(ptk != NULL){

        sv = PushBackString(sv, ptk);

        ptk = strtok(NULL, delim);
                
    }

    printf("vector filled\n");

    row = sv->size;

    printf("row num: %d, row: %d\n", row_num, row);



    for(int i = 0; i < row; i ++){

        printf("[ LINE ]: %s\n", sv->vector[i]);

    }


    printf("[ EOF ]\n");


    for(int i = 0; i < row; i ++){

        free(sv->vector[i]);


    }

    free(sv->vector);
    free(sv);

}


struct VectorString* PushBackString(struct VectorString* sv, char* new_str){

    int org_vec_size = sv->size;

    int new_str_len = strlen(new_str);

    int new_vec_size = org_vec_size + 1;

    struct VectorString* new_vec = (struct VectorString*)malloc(sizeof(struct VectorString));

    new_vec->vector = (char**)malloc(new_vec_size * sizeof(char*));

    for(int i = 0 ; i < org_vec_size; i++){

        int line_strlen = strlen(sv->vector[i]) + 1;

        new_vec->vector[i] = (char*)malloc(line_strlen * sizeof(char));

        memset(new_vec->vector[i], 0, line_strlen * sizeof(char));

        strcpy(new_vec->vector[i], sv->vector[i]);

        free(sv->vector[i]);
    }

    free(sv->vector);

    free(sv);


    int new_line_strlen = strlen(new_str) + 1;

    new_vec->vector[org_vec_size] = (char*)malloc(new_line_strlen*sizeof(char));

    memset(new_vec->vector[org_vec_size], 0 , new_line_strlen * sizeof(char));

    strcpy(new_vec->vector[org_vec_size], new_str);

    new_vec->size = new_vec_size;


    return new_vec;
}

