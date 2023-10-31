#ifndef GH_BOT_PKG_BOT
#define GH_BOT_PKG_BOT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <time.h>
#include <stdlib.h>

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

    cJSON_ArrayForEach(resolution, assignees){
        
        cJSON *object = cJSON_GetArrayItem(assignees,arr_idx);

        cJSON *login = cJSON_GetObjectItemCaseSensitive(object,"login");

        char *assignee = (char *)malloc(MAX_OWNERS_LEN);

        memset(assignee,'\0',MAX_OWNERS_LEN);

        sprintf(assignee, "%s", login->valuestring);

        assignee_line[arr_idx] = assignee;    

        arr_idx += 1;

    }

    return arr_idx;


}


int get_pr_review_request(cJSON *arr, char** review_request_line){


    cJSON *rev_reqs = cJSON_GetObjectItemCaseSensitive(arr,"reviewRequests");

    cJSON *resolution = NULL;

    int arr_idx = 0;


    cJSON_ArrayForEach(resolution, rev_reqs){

        cJSON *object = cJSON_GetArrayItem(rev_reqs,arr_idx);

        cJSON *login = cJSON_GetObjectItemCaseSensitive(object,"login");

        char *rev_req = (char *)malloc(MAX_OWNERS_LEN);

        memset(rev_req,'\0',MAX_OWNERS_LEN);

        sprintf(rev_req, "%s", login->valuestring);

        review_request_line[arr_idx] = rev_req;    

        arr_idx += 1;

    }

    return arr_idx;



}


int get_pr_reviews(cJSON *arr, char** reviews_line){


    cJSON *rev_reqs = cJSON_GetObjectItemCaseSensitive(arr,"reviews");

    cJSON *resolution = NULL;

    int arr_idx = 0;

    cJSON_ArrayForEach(resolution, rev_reqs){
        
        cJSON *object = cJSON_GetArrayItem(rev_reqs,arr_idx);

        cJSON *state = cJSON_GetObjectItemCaseSensitive(object,"state");

        char *rev = (char *)malloc(MAX_OWNERS_LEN);

        memset(rev,'\0',MAX_OWNERS_LEN);

        sprintf(rev, "%s", state->valuestring);

        reviews_line[arr_idx] = rev;    

        arr_idx += 1;

    }

    return arr_idx;



}


int check_if_pr_opened(cJSON *arr){

    int open = 0;

    char *prstate = get_pr_state(arr);

    if (strcmp(prstate, "OPEN") == 0) {

        open = 1;

    }

    free(prstate);

    return open;

}



int check_if_assignee_exists(cJSON *arr){

    char **assignee_line = (char **)malloc(MAX_OWNERS_LEN);

    int assignee_count = get_pr_assignee(arr, assignee_line);


    for(int i=0;i < assignee_count;i++){

    //    printf("assignee: %s\n", assignee_line[i]);

        free(assignee_line[i]);

    }

    free(assignee_line);

    if (assignee_count < 1){
        return 0;
    } else {
        return 1;
    }
    

}

int check_if_review_request_exists(cJSON *arr){

    char **review_request_line = (char **)malloc(MAX_OWNERS_LEN);

    int rev_req_count = get_pr_review_request(arr, review_request_line);

    for(int i=0;i < rev_req_count;i++){

    //    printf("reviewer: %s\n", review_request_line[i]);

        free(review_request_line[i]);

    }

    free(review_request_line);

    if (rev_req_count < 1){
        return 0;
    } else {
        return 1;
    }

    
}

int check_if_review_exists(cJSON *arr){

    char **reviews_line = (char **)malloc(MAX_OWNERS_LEN);

    int rev_count = get_pr_reviews(arr, reviews_line);

    for(int i=0;i < rev_count;i++){

    //    printf("review state: %s\n", reviews_line[i]);

        free(reviews_line[i]);

    }

    free(reviews_line);

    if (rev_count < 1){
        return 0;
    } else {
        return 1;
    }

    
}

int check_if_change_requested(cJSON *arr){

    char **reviews_line = (char **)malloc(MAX_OWNERS_LEN);

    int rev_count = get_pr_reviews(arr, reviews_line);

    int change_requested = 0;

    for(int i=0;i < rev_count;i++){

    //    printf("review state: %s\n", reviews_line[i]);

        if(strcmp(reviews_line[i], "CHANGES_REQUESTED")==0){
            change_requested = 1;
        }

        free(reviews_line[i]);

    }

    free(reviews_line);

    if (change_requested != 1){
        return 0;
    } else {
        return 1;
    }

}

int check_if_mergeable(cJSON *arr){

    int mergeable_check = 0;

    cJSON *mergeable = cJSON_GetObjectItemCaseSensitive(arr,"mergeable");

    if(strcmp(mergeable->valuestring,"MERGEABLE") == 0){

        mergeable_check = 1;
    } 

    return mergeable_check;

}
