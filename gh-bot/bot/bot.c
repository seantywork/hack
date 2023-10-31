#ifndef GH_BOT_PKG_BOT
#define GH_BOT_PKG_BOT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <cjson/cJSON.h>


#include "../glob.h"
#include "../utils.h"
#include "actions.c"
#include "checks.c"

#endif



int get_owners(char owners[MAX_OWNERS_LEN][MAX_OWNERS_LEN]){

    FILE *fileptr;

    char tmp_line[MAX_OWNERS_LEN] = {0};



    int owners_count = 0;

    fileptr = fopen("OWNERS","r");

    while(fgets(tmp_line, sizeof(tmp_line), fileptr) != NULL){

        if(strcmp(tmp_line, "") == 0){
            continue;
        }


        if(strcmp(tmp_line, " ") == 0){
            continue;
        }

        if(strcmp(tmp_line, "\n") == 0){
            continue;
        }

        char *owner = get_owner_without_newline(tmp_line);

        strcpy(owners[owners_count], owner);


        free(owner);

     
        owners_count += 1;
    }


    pclose(fileptr);



    return owners_count;
}


int get_pr_list(char *pr_line, char pr_list[MAX_PR_LIST_LEN][MAX_PR_LIST_LEN]){


    cJSON *arr = cJSON_Parse(pr_line);

    cJSON *resolution = NULL;

    int arr_idx = 0;

    int size = 0;

    int pr_length = 0; 

    cJSON_ArrayForEach(resolution, arr){
        
        cJSON *object = cJSON_GetArrayItem(arr,arr_idx);

        cJSON *number = cJSON_GetObjectItemCaseSensitive(object,"number");

        char pr_num[MAX_PR_NUMBER] = "";

        sprintf(pr_num, "%d", number->valueint);

        strcpy(pr_list[arr_idx],pr_num);    

        arr_idx += 1;

    }

    pr_length = arr_idx;

    cJSON_Delete(arr);

    return pr_length;

}

char* branch_by_pr_phase(char* pr_num, int owners_count, char owners[MAX_OWNERS_LEN][MAX_OWNERS_LEN], char target_author[MAX_OWNERS_LEN]){

    FILE *fp;
    char* ret = malloc(MAX_PHASE_FLAG_LEN);
    memset(ret, '\0', MAX_PHASE_FLAG_LEN);
    cJSON *arr;
    char* author;
    
    int is_open = 0;
    int is_member = 0;
    int is_assigned = 0;
    int has_review_requests = 0;
    int has_reviews = 0;
    int has_change_request = 0;
    int is_mergeable = 0;
    char json_line[MAX_JSON_STRLEN] = {0};

    char dest_command[MAX_COMMAND_LEN] = {0};
    char* source_command = "gh pr view %s --json state,author,assignees,reviews,reviewRequests,mergeable";

    sprintf(dest_command, source_command, pr_num);

    fp = popen(dest_command, "r");
    if (fp == NULL) {
        printf("failed to run command: check\n" );
        exit(1);
    }

    // while (fgets(tmp_line, sizeof(tmp_line), fp) != NULL) {
    //    strcpy(ret, tmp_line);    
    // }

    printf("  pr number: %s\n",pr_num);

    char* P = fgets(json_line, sizeof(json_line), fp);

    if (P == NULL){

        pclose(fp);

        strcpy(ret, "ERRJSON");

        return ret;

    }

    arr = cJSON_Parse(json_line);

    is_open = check_if_pr_opened(arr);

    printf("  prstate: %d\n", is_open);

    author = get_pr_author(arr);

    printf("  author: %s\n", author);

    is_member = check_if_author_in_owners(author, owners_count, owners);

    printf("  - is member: %d\n",is_member);

    is_assigned = check_if_assignee_exists(arr);

    printf("  - is assigned: %d\n",is_assigned);

    has_review_requests = check_if_review_request_exists(arr);

    printf("  - has review request: %d\n", has_review_requests);

    has_reviews = check_if_review_exists(arr);

    printf("  - has review: %d\n", has_reviews);

    has_change_request = check_if_change_requested(arr);

    printf("  - has change request: %d\n",has_change_request);

    is_mergeable = check_if_mergeable(arr);

    printf("  - is mergeable: %d\n",is_mergeable);

    if (is_open != 1){

        strcpy(ret, "CLOSED");

        printf("  - action: %s\n",ret);

    } else if (is_mergeable != 1){

        strcpy(ret, "MCCLOSE");

        printf("  - action: %s\n",ret);

    } else if (has_change_request == 1){

        strcpy(ret, "CRCLOSE");

        printf("  - action: %s\n",ret);

    } else if (is_assigned != 1 && has_review_requests != 1 && has_reviews != 1){

        if (is_member == 1){
            strcpy(ret, "REVAS");    
        } else if (is_member != 1){
            strcpy(ret, "REVASOUT");    
        }

        printf("  - action: %s\n",ret);
    
    } else if (is_assigned == 1 && has_review_requests != 1 && has_reviews != 1){


        if (is_member == 1){
            strcpy(ret, "REV");    
        } else if (is_member != 1){
            strcpy(ret, "REVOUT");    
        }

        printf("  - action: %s\n",ret);

    } else if (is_assigned != 1 && (has_review_requests == 1 || has_reviews == 1)){

        if (is_member == 1){
            strcpy(ret, "AS");    
        } else if (is_member != 1){
            strcpy(ret, "ASOUT");    
        }

        printf("  - action: %s\n",ret);

    } else if (is_assigned == 1 && has_review_requests == 0 && has_reviews == 1){

        strcpy(ret, "MERGE");

        printf("  - action: %s\n",ret);
    }


    strcpy(target_author, author);

    free(author);

    cJSON_Delete(arr);

    pclose(fp);

    return ret;

}



void gh_bot_iteration(){

    FILE *fp;
    char line[MAX_LINE_LEN];
    int owners_count = 0;
    int pr_length = 0;
    char owners[MAX_OWNERS_LEN][MAX_OWNERS_LEN] = {{0}};
    char pr_list[MAX_PR_LIST_LEN][MAX_PR_LIST_LEN] = {{0}};
    char author[MAX_OWNERS_LEN] = {0};

    owners_count = get_owners(owners);

    printf("owners: %d\n",owners_count);

    for (int i=0;i<owners_count; i++){

        printf("- owner: %s\n", owners[i]);

    }

    fp = popen("gh pr list --json number", "r");
    if (fp == NULL) {
        printf("failed to run command: list\n" );
        exit(1);
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
 
        pr_length = get_pr_list(line, pr_list);
    }

    printf("pending pr list: \n");

    for (int i=0;i<pr_length; i++){

        printf("- pr: %s\n", pr_list[i]);

    }
    
    printf("entering each pr...\n");

    for (int i=0; i<pr_length; i++){

        char* pr_phase = branch_by_pr_phase(pr_list[i], owners_count, owners, author);

        pthread_t thread_id;

        ActionStruct action_struct;

        if (strcmp(pr_phase, "CLOSED")==0) {

//            strcpy(action_struct.PRNumber, pr_list[i]);

//            strcpy(action_struct.Message, pr_phase);

//            pthread_create(&thread_id, NULL, get_and_write, (void *)&action_struct);

            printf("pr %s already closed\n",pr_list[i]);

        } else if (strcmp(pr_phase, "MCCLOSE") == 0){

            strcpy(action_struct.PRNumber, pr_list[i]);

            strcpy(action_struct.Message, pr_phase);

            pthread_create(&thread_id, NULL, close_pr, (void *)&action_struct);

            printf("pr %s merge conflict, force closed\n",pr_list[i]);

        } else if (strcmp(pr_phase, "CRCLOSE") == 0){

            strcpy(action_struct.PRNumber, pr_list[i]);

            strcpy(action_struct.Message, pr_phase);

            pthread_create(&thread_id, NULL, close_pr, (void *)&action_struct);

            printf("pr %s reject, force closed\n",pr_list[i]);

        } else if (strcmp(pr_phase, "REVAS") == 0){

            strcpy(action_struct.PRNumber, pr_list[i]);

            strcpy(action_struct.Message, pr_phase);

            fill_action_struct(&action_struct, owners_count, owners, author);

            pthread_create(&thread_id, NULL, add_pr_request_and_assignee, (void *)&action_struct);

            printf("pr %s needs assignee and reviewer for member\n",pr_list[i]);

        } else if (strcmp(pr_phase, "REVASOUT") == 0){
 
            strcpy(action_struct.PRNumber, pr_list[i]);

            strcpy(action_struct.Message, pr_phase);

            fill_action_struct_for_outsider(&action_struct, owners_count, owners);
       
            pthread_create(&thread_id, NULL, add_pr_request_and_assignee, (void *)&action_struct);

            printf("pr %s needs assignee and reviewer for outsider\n",pr_list[i]);

        } else if (strcmp(pr_phase, "REV") == 0){

            strcpy(action_struct.PRNumber, pr_list[i]);

            strcpy(action_struct.Message, pr_phase);

            fill_action_struct(&action_struct, owners_count, owners, author);

            pthread_create(&thread_id, NULL, add_pr_request, (void *)&action_struct);

            printf("pr %s needs reviewer for member\n",pr_list[i]);

        } else if (strcmp(pr_phase, "REVOUT") == 0){

            strcpy(action_struct.PRNumber, pr_list[i]);

            strcpy(action_struct.Message, pr_phase);

            fill_action_struct_for_outsider(&action_struct, owners_count, owners);

            pthread_create(&thread_id, NULL, add_pr_request, (void *)&action_struct);

            printf("pr %s needs reviewer for outsider\n",pr_list[i]);

        } else if (strcmp(pr_phase, "AS") == 0){

            strcpy(action_struct.PRNumber, pr_list[i]);

            strcpy(action_struct.Message, pr_phase);

            fill_action_struct(&action_struct, owners_count, owners, author);

            pthread_create(&thread_id, NULL, add_pr_assignee, (void *)&action_struct);

            printf("pr %s needs assignee for member\n",pr_list[i]);

        } else if (strcmp(pr_phase, "ASOUT") == 0){

            strcpy(action_struct.PRNumber, pr_list[i]);

            strcpy(action_struct.Message, pr_phase);

            fill_action_struct_for_outsider(&action_struct, owners_count, owners);

            pthread_create(&thread_id, NULL, add_pr_assignee, (void *)&action_struct);

            printf("pr %s needs assignee for outsider\n",pr_list[i]);

        } else if (strcmp(pr_phase, "MERGE") == 0){

            strcpy(action_struct.PRNumber, pr_list[i]);

            strcpy(action_struct.Message, pr_phase);

            pthread_create(&thread_id, NULL, merge_pr, (void *)&action_struct);

            printf("pr %s merge\n",pr_list[i]);

        }

        free(pr_phase);

    }



    pclose(fp);

    sleep(10);
    //pclose(fp_owners);

}