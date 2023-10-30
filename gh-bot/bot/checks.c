#ifndef GH_BOT_PKG_BOT
#define GH_BOT_PKG_BOT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "../glob.h"
#endif


char* get_owner_without_newline(char* owner_nl){

    char* ret_owner;

    char* delim = "\n";

    char* token = strtok(owner_nl, delim);

    size_t owner_nl_length = sizeof(owner_nl);

    ret_owner = malloc(owner_nl_length);

    strcpy(ret_owner, token);

    return ret_owner;
}   

char* get_pr_state(cJSON *arr){


    char* ret_state;

    cJSON *state = cJSON_GetObjectItemCaseSensitive(arr,"state");

    ret_state = (char *)malloc(sizeof(state->valuestring));

    strcpy(ret_state, state->valuestring);

    return ret_state;


}

char* get_pr_author(cJSON *arr){

    char* ret_author;

    cJSON *author = cJSON_GetObjectItemCaseSensitive(arr,"author");

    cJSON *login = cJSON_GetObjectItemCaseSensitive(author,"login");

    ret_author = (char *)malloc(sizeof(login->valuestring));

    strcpy(ret_author, login->valuestring);

    return ret_author;

}


int check_if_author_in_owners(char* author, int owner_count, char owners[MAX_OWNERS_LEN][MAX_OWNERS_LEN]){

    int hit = 0;

    for(int i =0 ;i < owner_count;i++){
        if(strcmp(author, owners[i]) == 0){
            hit = 1;
            break;
        }
    }

    return hit;

}

int get_pr_assignee(cJSON *arr, char** assignee_line){

    cJSON *assignees = cJSON_GetObjectItemCaseSensitive(arr,"assignees");

    cJSON *resolution = NULL;

    int arr_idx = 0;

    char *assignee;

    cJSON_ArrayForEach(resolution, assignees){
        
        cJSON *object = cJSON_GetArrayItem(assignees,arr_idx);

        cJSON *login = cJSON_GetObjectItemCaseSensitive(object,"login");

        assignee = (char *)malloc(MAX_OWNERS_LEN);

        memset(assignee,'\0',MAX_OWNERS_LEN);

        sprintf(assignee, "%s", login->valuestring);

        assignee_line[arr_idx] = assignee;    

        arr_idx += 1;

    }

    return arr_idx;


}


char* get_pr_review_request(cJSON *arr){





}


char* get_pr_reviews(cJSON *arr){






}




int check_if_assignee_exists(cJSON *arr){

    char **assignee_line = (char **)malloc(MAX_OWNERS_LEN);

    int assignee_count = get_pr_assignee(arr, assignee_line);


    for(int i=0;i < assignee_count;i++){

        printf("assignee: %s\n", assignee_line[i]);

        free(assignee_line[i]);

    }

    free(assignee_line);

    if (assignee_count < 1){
        return 0;
    } else {
        return 1;
    }
    

}

int check_if_reviewer_exists(){


    
}
