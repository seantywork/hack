#ifndef GH_BOT_PKG_BOT
#define GH_BOT_PKG_BOT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "../glob.h"
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

char* branch_by_pr_phase(char* pr_num, int owners_count, char owners[MAX_OWNERS_LEN][MAX_OWNERS_LEN]){

    FILE *fp;
    char* ret = malloc(MAX_PHASE_FLAG_LEN);
    memset(ret, '\0', MAX_PHASE_FLAG_LEN);
    cJSON *arr;
    char* prstate;
    char* author;

    int is_member = 0;
    int is_assigned = 0;
    char json_line[MAX_JSON_STRLEN] = {0};

    char dest_command[MAX_COMMAND_LEN] = {0};
    char* source_command = "gh pr view %s --json state,author,assignees,reviews,reviewRequests";

    sprintf(dest_command, source_command, pr_num);

    fp = popen(dest_command, "r");
    if (fp == NULL) {
        printf("failed to run command: check\n" );
        exit(1);
    }

    // while (fgets(tmp_line, sizeof(tmp_line), fp) != NULL) {
    //    strcpy(ret, tmp_line);    
    // }

    char* P = fgets(json_line, sizeof(json_line), fp);

    if (P == NULL){

        pclose(fp);

        strcpy(ret, "ERRJSON");

        return ret;

    }

    arr = cJSON_Parse(json_line);

    prstate = get_pr_state(arr);

    printf("prstate: %s\n", prstate);

    if (strcmp(prstate, "OPEN") != 0) {

        cJSON_Delete(arr);

        pclose(fp);

        strcpy(ret, "CLOSED");

        return ret;

    }

    author = get_pr_author(arr);

    printf("author: %s\n", author);


    is_member = check_if_author_in_owners(author, owners_count, owners);

    printf("is member: %d\n",is_member);

    is_assigned = check_if_assignee_exists(arr);

    printf("is assigned: %d\n",is_assigned);






    //strcpy(ret, "ADDRV");

    //strcpy(ret, "ADDAS");

    strcpy(ret, "MERGE");


    free(author);

    free(prstate);

    cJSON_Delete(arr);

    pclose(fp);

    return ret;

}



void gh_bot_iteration(){

    FILE *fp;
    FILE *fp_owners;
    char line[MAX_LINE_LEN];
    int owners_count = 0;
    int pr_length = 0;
    char owners[MAX_OWNERS_LEN][MAX_OWNERS_LEN] = {{0}};
    char pr_list[MAX_PR_LIST_LEN][MAX_PR_LIST_LEN] = {{0}};

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

        char* pr_phase = branch_by_pr_phase(pr_list[i], owners_count, owners);

        printf("%s\n",pr_phase);

        free(pr_phase);

    }



    pclose(fp);
    //pclose(fp_owners);

}