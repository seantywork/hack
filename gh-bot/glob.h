

#ifndef GH_BOT_GLOB
#define GH_BOT_GLOB

#define MAX_LINE_LEN 1024
#define MAX_OWNERS_LEN 512
#define MAX_COMMAND_LEN 1024
#define MAX_PR_LIST_LEN 1024
#define MAX_PR_NUMBER 256
#define MAX_JSON_STRLEN 1024 * 128
#define MAX_PHASE_FLAG_LEN 10
#define MAX_MESSAGE_LEN 10
#define MAX_PATH_LEN 1024
#define MAX_CONTENT_LEN 1024 * 512

struct ActionStruct {

    char PRNumber[MAX_PR_NUMBER];
    char Reviewer[MAX_OWNERS_LEN];
    char Assignee[MAX_OWNERS_LEN];
    char Message[MAX_MESSAGE_LEN];

};

typedef struct ActionStruct ActionStruct;

#endif