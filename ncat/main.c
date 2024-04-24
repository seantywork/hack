#include "ncat.h"

int ncat_argc;
char** ncat_argv;
NCAT_OPTIONS NCATOPTS;
int ncat_connect = 0;
int ncat_listen = 0;
int serve_content_exists = 0;
char serve_content[MAX_LOAD_BUFF] = {0};
int serve_content_ptr = 0;

int main(int argc, char** argv){


    if(argc < 2){
        fprintf(stderr, "too few arguments\n");
        return -1;
    }

    if(signal(SIGINT, NCAT_keyboard_interrupt) == SIG_ERR){

        fprintf(stderr, "failed to add interrupt handler\n");
        return -1;

    }

    if (pthread_mutex_init(&NCATOPTS._lock, NULL) != 0) { 
        fprintf(stderr,"mutex init failed\n"); 
        return -1; 
    } 
  


    ncat_argc = argc - 1;

    ncat_argv = (char**)malloc(ncat_argc * sizeof(char*));



    for(int i = 0; i < ncat_argc; i ++){

        int idx = i + 1;

        int option_len = strlen(argv[idx]) + 1;

        ncat_argv[i] = (char*)malloc(option_len * sizeof(char));

        memset(ncat_argv[i], 0, option_len * sizeof(char));

        strcpy(ncat_argv[i], argv[idx]);

    }



    int parsed = NCAT_parse_args(ncat_argc, ncat_argv);

    if(parsed < 0){

        

        fprintf(stderr, "failed to parse arg\n");

        NCAT_free();

        return parsed;

    }

    NCATOPTS._client_sock_ready = 0;
    NCATOPTS._client_sockfd = -1;


    int result = NCAT_runner();

    if(result < 0){

        fprintf(stderr, "failed to run command\n");

        NCAT_free();

        return result;

    }



    return 0;
}