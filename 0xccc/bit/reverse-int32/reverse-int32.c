#include "bit/c.h"



#define MAX_INT32_STRLEN 12


char* LOWER_LIMIT_INT32_STR = "-2147483648";

char* UPPER_LIMIT_INT32_STR = "2147483647";


int CheckInt32StrValidity(char* a){

    int negative = 0;

    if(a[0] == '-'){
        negative = 1;
    }

    char* b;

    if(negative == 1){

        b = LOWER_LIMIT_INT32_STR;


        if(strlen(a) > strlen(b)){
            return -1;
        }

        if(strlen(a) < strlen(b)){
            return 0;
        }

    } else {

        b = UPPER_LIMIT_INT32_STR;

        if(strlen(a) > strlen(b)){
            return -1;
        }

        if (strlen(a) < strlen(b)){
            return 0;
        }

    }

    int idx = 0;

    int total_len = strlen(a);

    if(negative == 1){
        idx += 1;
    }



    for(; idx < total_len; idx++){


        int a_int  = (int)a[idx];
        int b_int = (int)b[idx];

        if(a_int == b_int){
            continue;
        } else if (a_int < b_int) {
            return 0;
        } else{
            return -1;
        }


    }


    return 1;

}


int ReverseInt32WithOverflowCheck(char* x_str){

    int xstr_len = strlen(x_str);

    int tail = xstr_len - 1;


    char test[MAX_INT32_STRLEN] = {0};

    int idx = 0;

    int overflow = 0;

    int ret = 0;

    if(x_str[0] == '-'){
        test[0] = '-';
        idx += 1;
    }

    if(x_str[tail] == '0'){
        xstr_len -= 1;
        tail -= 1;
    }

    for(; idx < xstr_len; idx++){

        test[idx] = x_str[tail];

        tail -= 1;

    }


    overflow = CheckInt32StrValidity(test);

    if(overflow == -1){
        ret = 0;
    } else {
        ret = atoi(test);
    }
    
    return ret;

}



int reverse(int x){

    char x_str[MAX_INT32_STRLEN] = {0};

    sprintf(x_str, "%d", x);

    int ret = 0;

    ret = ReverseInt32WithOverflowCheck(x_str);

    return ret;

}