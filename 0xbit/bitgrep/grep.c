#include "bitgrep/grep.h"




BITGREP_FLAG BITGREP_FlagParser(BITGREP_TARGET* bg_t, int argc, char** argv){

    if(argc != 2){

        return EARG;

    }


    int find_arg_len = strlen(argv[1]) + 1;

    bg_t->target_string = (char*)malloc(find_arg_len * sizeof(char));

    memset(bg_t->target_string, 0 , find_arg_len * sizeof(char));

    strcpy(bg_t->target_string, argv[1]);


    return FIND;


}




int BITGREP_MapStdinTo2d(BITGREP_INPUT2D* bg_in2d){


    size_t total_input_size = 1;

    char get_buffer[MAX_STDINPUT_WIDTH] = {0}; 

    char* buff_input = (char*)malloc(MAX_STDINPUT_WIDTH * sizeof(char));

    memset(buff_input, 0 , MAX_STDINPUT_WIDTH * sizeof(char));

    while(fgets(get_buffer, MAX_STDINPUT_WIDTH, stdin)){
 
        char* old_buff_input = buff_input;
 
        total_input_size += strlen(get_buffer);
        
        buff_input = realloc(buff_input, total_input_size);

        if(buff_input == NULL){

            free(old_buff_input);
            
            return -1;
        }

        strcat(buff_input, get_buffer);

    }

    if(ferror(stdin)){

        free(buff_input);
        
        return -2;
    }




    char* delim = "\n";  

    char* line_ptr;    


    while ((line_ptr = strsep(&buff_input, delim)) != NULL) {  


        int status = BITGREP_PushBackString(bg_in2d->row_count, &bg_in2d->buff_2d, line_ptr);

        if(status < -1){

            return -3;
        }

        bg_in2d->row_count += 1;

    }
  
    free(buff_input);


    return 0;

}

int BITGREP_PushBackString(int rowc, char*** vec, char* new_el){

    int origin_row_count = rowc;

    int new_row_count = origin_row_count + 1;

    char** vec_tmp = (char**)malloc(new_row_count * sizeof(char*));

    for(int i = 0; i < origin_row_count; i ++){

        int line_len = strlen((*vec)[i]) + 1;

        vec_tmp[i] = (char*)malloc(line_len * sizeof(char) + 1);

        memset(vec_tmp[i], 0 , line_len * sizeof(char) + 1);

        strcpy(vec_tmp[i], (*vec)[i]);

        free((*vec)[i]);

       //printf("[ COPIED %d ] %s\n", i, vec_tmp[i]);

    }

    int new_line_len = strlen(new_el);

    vec_tmp[origin_row_count] = (char*)malloc(new_line_len * sizeof(char) + 1);

    memset(vec_tmp[origin_row_count], 0 , new_line_len * sizeof(char) + 1);

    strcpy(vec_tmp[origin_row_count], new_el);

    if(origin_row_count != 0){
        free(*vec);
    }

    *vec = vec_tmp;


    return 0;
}

int BITGREP_PushBackInt(int count, int** vec, int new_el){

    int origin_el_count = count;

    int new_el_count = origin_el_count + 1;

    int* vec_tmp = (int*)malloc(new_el_count * sizeof(int));

    for(int i = 0; i < origin_el_count; i ++){


        vec_tmp[i] = (*vec)[i];

       //printf("[ COPIED %d ] %s\n", i, vec_tmp[i]);

    }

    vec_tmp[origin_el_count] = new_el;
   
    if(origin_el_count != 0){
        free(*vec);
    }

    *vec = vec_tmp;


    return 0;
}


int BITGREP_PopulateSearchResult(char* target, BITGREP_INPUT2D* bg_in2d, BITGREP_FOUND* bg_found){


    for(int i = 0; i < bg_in2d->row_count; i++){

        char* target_found;

        int status = 0;

        if((target_found = strstr(bg_in2d->buff_2d[i], target)) != NULL){

            status = BITGREP_PushBackInt(bg_found->found_count, &bg_found->found_index, i);

            if(status < 0){

                return -1;
            }

            status = BITGREP_PushBackString(bg_found->found_count, &bg_found->buff_found, target_found);

            if(status < 0){

                return -2;
            }

            bg_found->found_count += 1;

        }


    }


    return 0;

}


void BITGREP_PrintFound(BITGREP_FOUND* bg_found){


    for(int i = 0; i < bg_found->found_count; i++){

        printf("[ %d ]: ", bg_found->found_index[i]);

        printf("%s\n", bg_found->buff_found[i]);

    }



}



void BITGREP_Clear(
    BITGREP_TARGET* bg_t, 
    BITGREP_INPUT2D* bg_in2d, 
    BITGREP_FOUND* bg_found
    ){


        free(bg_t->target_string);

        for(int i = 0; i < bg_in2d->row_count; i++){

            free(bg_in2d->buff_2d[i]);

        }

        free(bg_in2d->buff_2d);

        for(int i = 0; i < bg_found->found_count; i++){

            free(bg_found->buff_found[i]);

        }

        free(bg_found->found_index);
        free(bg_found->buff_found);
}