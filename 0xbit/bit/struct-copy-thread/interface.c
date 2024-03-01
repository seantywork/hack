#include "bit/c.h"


void StructCopyThreadInterface(){


    int done = 0;

    pthread_t tid1;

    pthread_t tid2;

    char comment_arr[MAX_MEMBER_ARR_LEN] = {0};

    strcpy(comment_arr, "comment for arr");

    char comment_ptr[MAX_MEMBER_ARR_LEN] = {0};

    strcpy(comment_ptr, "comment for ptr");

    /*
        comment pointer not transferred
    struct StructWithArr sarr = {
        .name = 0,
        .comment = comment_arr
    };
    */

    struct StructWithArr sarr;

    sarr.name = 0;
    
    strcpy(sarr.comment, comment_arr);

    struct StructWithPtr sptr = {
        .name = 1,
        .comment = comment_ptr
    };

    printf("start: comment for arr: %s\n", sarr.comment);

    printf("start: comment for ptr: %s\n", sptr.comment);

    pthread_create(&tid1, NULL, StructCopyThreadForSarr, (void*)&sarr);

    pthread_create(&tid2, NULL, StructCopyThreadForSptr, (void*)&sptr);


    while(done == 0){

        printf("hit 1 to stop: ");

        scanf("%d", &done);

    }


    printf("end: comment for arr: %s\n", sarr.comment);

    printf("end: comment for ptr: %s\n", comment_ptr);

}