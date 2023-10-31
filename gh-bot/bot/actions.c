
#ifndef GH_BOT_PKG_BOT
#define GH_BOT_PKG_BOT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <time.h>

#include "../glob.h"
#include "../utils.h"


#endif


void fill_action_struct(ActionStruct* action_struct, int owners_count,char owners[MAX_OWNERS_LEN][MAX_OWNERS_LEN], char author[MAX_OWNERS_LEN]){


    char** new_owners = (char **)malloc(sizeof(char **) * (owners_count - 1));

    int new_counter = 0;

    for(int i = 0; i < owners_count; i++){

        if(strcmp(owners[i],author) == 0){

            continue;
        }

        new_owners[new_counter] = (char *)malloc(MAX_OWNERS_LEN);

        memset(new_owners[new_counter], '\0', MAX_OWNERS_LEN);

        strcpy(new_owners[new_counter], owners[i]);

        new_counter += 1;
    }    


    int upper = new_counter - 1;

    int idx = GetRandNumInRange(0, upper);

    strcpy(action_struct->Reviewer, new_owners[idx]);

    strcpy(action_struct->Assignee, author);

    for(int i = 0; i < new_counter; i++){

        free(new_owners[i]);

    }    

    free(new_owners);

}

void fill_action_struct_for_outsider(ActionStruct* action_struct, int owners_count,char owners[MAX_OWNERS_LEN][MAX_OWNERS_LEN]){

    int upper = owners_count - 1;

    int idx = GetRandNumInRange(0, upper);

    strcpy(action_struct->Reviewer, owners[idx]);

    strcpy(action_struct->Assignee, owners[idx]);

}

void write_message(char* id, char *message){

    FILE *fptr;

    char file_name[MAX_PATH_LEN] = {0};

    char* file_name_base = "log/gh-bot-action-%s";

    sprintf(file_name, file_name_base, id);

    fptr = fopen(file_name,"a");

    fprintf(fptr, "%s\n", message);

    fclose(fptr);

}



void* add_pr_request_and_assignee(void* void_struct){

    FILE *fp;

    char message[MAX_CONTENT_LEN] = {0};

    ActionStruct* action_struct = (ActionStruct *)void_struct;

    char dest_command[MAX_COMMAND_LEN] = {0};
    char* source_command_rev = "gh pr edit %s --add-reviewer %s";
    char* source_command_as = "gh pr edit %s --add-assignee %s";

    sprintf(dest_command, source_command_rev, action_struct->PRNumber, action_struct->Reviewer);

    fp = popen(dest_command, "r");
    if (fp == NULL) {
        printf("failed to run command: add reviewer\n" );
        exit(1);
    }

    strcpy(message, dest_command);

    write_message(action_struct->PRNumber, message);

    memset(dest_command, '\0',MAX_COMMAND_LEN);

    sprintf(dest_command, source_command_as, action_struct->PRNumber, action_struct->Assignee);

    fp = popen(dest_command, "r");
    if (fp == NULL) {
        printf("failed to run command: add assignee\n" );
        exit(1);
    }

    strcpy(message, dest_command);

    write_message(action_struct->PRNumber, message);

    pclose(fp);
}


void* add_pr_request(void* void_struct){

    FILE *fp;

    char message[MAX_CONTENT_LEN] = {0};

    ActionStruct* action_struct = (ActionStruct *)void_struct;

    char dest_command[MAX_COMMAND_LEN] = {0};
    char* source_command_rev = "gh pr edit %s --add-reviewer %s";


    sprintf(dest_command, source_command_rev, action_struct->PRNumber, action_struct->Reviewer);

    fp = popen(dest_command, "r");
    if (fp == NULL) {
        printf("failed to run command: add reviewer\n" );
        exit(1);
    }

    strcpy(message, dest_command);

    write_message(action_struct->PRNumber, message);

    pclose(fp);
}


void* add_pr_assignee(void* void_struct){

    FILE *fp;

    char message[MAX_CONTENT_LEN] = {0};

    ActionStruct* action_struct = (ActionStruct *)void_struct;

    char dest_command[MAX_COMMAND_LEN] = {0};

    char* source_command_as = "gh pr edit %s --add-assignee %s";

    sprintf(dest_command, source_command_as, action_struct->PRNumber, action_struct->Assignee);

    fp = popen(dest_command, "r");
    if (fp == NULL) {
        printf("failed to run command: add assignee\n" );
        exit(1);
    }


    strcpy(message, dest_command);

    write_message(action_struct->PRNumber, message);

    pclose(fp);
}


void* close_pr(void* void_struct){

    FILE *fp;

    char message[MAX_CONTENT_LEN] = {0};

    ActionStruct* action_struct = (ActionStruct *)void_struct;

    char dest_command[MAX_COMMAND_LEN] = {0};

    char* source_command_as = "gh pr close %s";

    sprintf(dest_command, source_command_as, action_struct->PRNumber);

    fp = popen(dest_command, "r");
    if (fp == NULL) {
        printf("failed to run command: close\n" );
        exit(1);
    }

    strcpy(message, dest_command);

    write_message(action_struct->PRNumber, message);

    pclose(fp);

}

void* merge_pr(void* void_struct){

    FILE *fp;

    char message[MAX_CONTENT_LEN] = {0};

    ActionStruct* action_struct = (ActionStruct *)void_struct;

    char dest_command[MAX_COMMAND_LEN] = {0};

    char* source_command_as = "gh pr merge %s --merge";

    sprintf(dest_command, source_command_as, action_struct->PRNumber);

    fp = popen(dest_command, "r");
    if (fp == NULL) {
        printf("failed to run command: merge\n" );
        exit(1);
    }

    strcpy(message, dest_command);

    write_message(action_struct->PRNumber, message);

    pclose(fp);
}
